#include <not_implemented.h>

#include "../include/allocator_red_black_tree.h"

allocator_red_black_tree::~allocator_red_black_tree()
{
	debug_with_guard(get_typename() + " destructor called");
	get_mutex().~mutex();
	deallocate_with_guard(_trusted_memory);
}


allocator_red_black_tree::allocator_red_black_tree(allocator_red_black_tree &&other) noexcept : _trusted_memory(other._trusted_memory)
{
	debug_with_guard("Start move constructor of " + get_typename());
	other._trusted_memory = nullptr;
}
allocator_red_black_tree &allocator_red_black_tree::operator=(
		allocator_red_black_tree &&other) noexcept
{
	debug_with_guard("Start assignment operator of " + get_typename());
	if (this != &other)
	{
		get_mutex().~mutex();
		deallocate_with_guard(_trusted_memory);

		_trusted_memory = other._trusted_memory;
		other._trusted_memory = nullptr;
	}
	return *this;
}

allocator_red_black_tree::allocator_red_black_tree(
		size_t space_size,
		allocator *parent_allocator,
		logger *logger,
		allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
	size_t allocator_size = space_size + get_allocator_size_of_meta();

	try
	{
		_trusted_memory = parent_allocator == nullptr ? ::operator new(allocator_size) : parent_allocator->allocate(allocator_size, 1);
	}
	catch (std::bad_alloc const & ex)
	{
		error_with_guard(get_typename() + " failed to allocate " + std::to_string(allocator_size) + " bytes of memory");
		throw ex;
	}

	auto ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

	*reinterpret_cast<class logger**>(ptr) = logger;

	ptr += sizeof(class logger*);

	*reinterpret_cast<allocator**>(ptr) = parent_allocator;

	ptr += sizeof(allocator*);

	*reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = allocate_fit_mode;

	ptr += sizeof(allocator_with_fit_mode::fit_mode);

	*reinterpret_cast<size_t*>(ptr) = space_size;

	ptr += sizeof(size_t);

	auto mutex = reinterpret_cast<std::mutex*>(ptr);
	construct(mutex);

	ptr += sizeof(std::mutex);

	auto first_forward = reinterpret_cast<void**>(ptr);
	*first_forward = reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_size_of_meta();

	ptr += sizeof(void*);
	auto first_free_block = reinterpret_cast<void*>(ptr);

	get_byte_occupied_color(first_free_block).is_occupied = false;
	get_byte_occupied_color(first_free_block)._color = color::BLACK;
	get_back_ptr(first_free_block) = nullptr;
	get_forward_ptr(first_free_block) = nullptr;
	get_parent(first_free_block) = nullptr;
	get_left_ptr(first_free_block) = nullptr;
	get_right_ptr(first_free_block) = nullptr;

	debug_with_guard(get_typename() + " constructor with parameters completed successfully");
}

[[nodiscard]] void *allocator_red_black_tree::allocate(
		size_t value_size,
		size_t values_count)
{
	std::lock_guard<std::mutex> lock(get_mutex());

	debug_with_guard(get_typename() + " allocate with value_size = " + std::to_string(value_size) + "; values_count = " +
					 std::to_string(values_count) + "; Now it is having " + std::to_string(get_free_size()) + " bytes");

	size_t need_mem = value_size * values_count;

	block_pointer_t find_new_free_block;

	switch(get_fit_mode())
	{
		case allocator_with_fit_mode::fit_mode::first_fit:
			find_new_free_block = get_first_suitable(need_mem);
			break;
		case allocator_with_fit_mode::fit_mode::the_best_fit:
			find_new_free_block = get_best_suitable(need_mem);
			break;
		case allocator_with_fit_mode::fit_mode::the_worst_fit:
			find_new_free_block = get_worst_suitable(need_mem);
			break;
	}

	if(find_new_free_block == nullptr)
	{
		error_with_guard(get_typename() + "No suitable block found for " + std::to_string(need_mem) + " bytes");
		throw std::bad_alloc();
	}

	remove_from_rb_tree(find_new_free_block);

	get_byte_occupied_color(find_new_free_block).is_occupied = true;

	get_parent(find_new_free_block) = _trusted_memory;

	size_t free_block_size = get_size_block(find_new_free_block, _trusted_memory);


	if(free_block_size < need_mem + get_free_block_size_of_meta())
	{
		need_mem = free_block_size;
		warning_with_guard("Allocator red black tree changed allocating block size to " + std::to_string(need_mem));
	}
	else
	{
		void* new_free = reinterpret_cast<unsigned char*>(find_new_free_block) + get_occupied_block_size_of_meta() + need_mem;

		get_forward_ptr(new_free) = get_forward_ptr(find_new_free_block);
		get_back_ptr(new_free) = find_new_free_block;
		get_forward_ptr(find_new_free_block) = new_free;
		if(get_forward_ptr(new_free) != nullptr)
		{
			get_back_ptr(get_forward_ptr(new_free)) = new_free;
		}
		get_byte_occupied_color(new_free).is_occupied = false;
		get_parent(new_free) = nullptr;

		insert_rb_tree(new_free);
	}


	debug_with_guard("Allocation completed. Allocated memory size: " + std::to_string(need_mem) + " bytes. ");
	information_with_guard(get_typename() + "current state of blocks: " + get_blocks_info_to_string(get_blocks_info()));

	return reinterpret_cast<unsigned char*>(find_new_free_block) + get_occupied_block_size_of_meta();
}


void allocator_red_black_tree::deallocate(
		void *at)
{
	std::lock_guard<std::mutex> lock(get_mutex());

	debug_with_guard("Start deallocate " + get_typename());

	void* block_ptr = reinterpret_cast<unsigned char*>(at) - get_occupied_block_size_of_meta();

	if(get_parent(block_ptr) != _trusted_memory)
	{
		error_with_guard("Invalid memory block");
		throw std::logic_error("Logic error: The received memory block does not belong to this allocator");
	}

	debug_with_guard("This block before deallocate " + get_dump(reinterpret_cast<char*>(at), get_size_block(block_ptr, _trusted_memory)));

	get_byte_occupied_color(block_ptr).is_occupied = false;

	//слияние с предыдущим
	if(get_back_ptr(block_ptr) != nullptr && !get_byte_occupied_color(get_back_ptr(block_ptr)).is_occupied)
	{
		void* tmp = block_ptr;
		block_ptr = get_back_ptr(block_ptr);
		remove_from_rb_tree(block_ptr);
		get_forward_ptr(block_ptr) = get_forward_ptr(tmp);
		if(get_forward_ptr(block_ptr) != nullptr)
		{
			get_back_ptr(get_forward_ptr(block_ptr)) = block_ptr;
		}
	}

	//слияние со следующим
	if(get_forward_ptr(block_ptr) != nullptr && !get_byte_occupied_color(get_forward_ptr(block_ptr)).is_occupied)
	{
		void* tmp = get_forward_ptr(block_ptr);
		remove_from_rb_tree(tmp);
		get_forward_ptr(block_ptr) = get_forward_ptr(tmp);
		if(get_forward_ptr(block_ptr) != nullptr)
		{
			get_back_ptr(get_forward_ptr(block_ptr)) = block_ptr;
		}
	}


	insert_rb_tree(block_ptr);

	trace_with_guard("deallocate from allocator finish");
	information_with_guard("current available memory: " + std::to_string(get_free_size()));
	information_with_guard(get_typename() + " current state of blocks: " + get_blocks_info_to_string(get_blocks_info()));
}


inline logger *allocator_red_black_tree::get_logger() const
{
	return *reinterpret_cast<logger**>(_trusted_memory);
}

inline allocator *allocator_red_black_tree::get_allocator() const
{
	auto ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(logger*);
	return *reinterpret_cast<allocator**>(ptr);
}

inline allocator_with_fit_mode::fit_mode& allocator_red_black_tree::get_fit_mode() const noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(logger*) + sizeof(allocator*);
	return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr);
}

inline size_t allocator_red_black_tree::get_size_full(void* trusted_m) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(trusted_m);
	ptr += sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode);
	return *reinterpret_cast<block_size_t *>(ptr);
}

inline std::mutex& allocator_red_black_tree::get_mutex() const noexcept
{
	auto ptr = reinterpret_cast<unsigned char *>(_trusted_memory);
	ptr += sizeof(logger*) + sizeof(allocator*)  + sizeof(fit_mode) + sizeof(size_t);
	return *reinterpret_cast<std::mutex*>(ptr);
}

void** allocator_red_black_tree::get_first_block(void* trusted_mem) noexcept
{
	auto ptr = reinterpret_cast<unsigned char *>(trusted_mem);
	ptr += sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex);
	return reinterpret_cast<block_pointer_t*>(ptr);
}

allocator_red_black_tree::byte_occupied_color& allocator_red_black_tree::get_byte_occupied_color(void* current_block) noexcept
{
	return *reinterpret_cast<byte_occupied_color*>(current_block);
}

void*& allocator_red_black_tree::get_back_ptr(void* current_block) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(current_block);
	ptr += sizeof(byte_occupied_color);
	return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_forward_ptr(void* current_block) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(current_block);
	ptr += sizeof(byte_occupied_color) + sizeof(void*);
	return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_parent(void* current_block) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(current_block);
	ptr += sizeof(byte_occupied_color) + 2 * sizeof(void*);
	return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_left_ptr(void* current_block) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(current_block);
	ptr += sizeof(byte_occupied_color) + 3 * sizeof(void*);

	return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_right_ptr(void* current_block) noexcept
{
	auto ptr = reinterpret_cast<unsigned char*>(current_block);
	ptr += sizeof(byte_occupied_color) + 4 * sizeof(void*);

	return *reinterpret_cast<void**>(ptr);
}

void* allocator_red_black_tree::get_first_suitable(size_t size) const noexcept
{
	void* result = *get_first_block(_trusted_memory);

	while(result != nullptr)
	{
		if(get_size_block(result, _trusted_memory) >= size)
		{
			return result;
		}
		result = get_right_ptr(result);
	}
	return result;
}

void* allocator_red_black_tree::get_worst_suitable(size_t size) const noexcept
{
	void* result = nullptr;
	void* node = *get_first_block(_trusted_memory);

	while(node != nullptr)
	{
		if(get_size_block(node, _trusted_memory) >= size)
		{
			result = node;
		}
		node = get_right_ptr(node);
	}

	return result;
}

void* allocator_red_black_tree::get_best_suitable(size_t size) const noexcept
{
	void* result = nullptr;
	void* node = *get_first_block(_trusted_memory);

	while(node != nullptr)
	{
		size_t size_of_node = get_size_block(node, _trusted_memory);
		if(size_of_node >= size)
		{
			result = node;
		}
		if(size_of_node > size)
		{
			node = get_left_ptr(node);
		}
		else if(size_of_node < size)
		{
			node = get_right_ptr(node);
		}
		else node = nullptr;
	}
	return result;
}


inline allocator::block_size_t allocator_red_black_tree::get_allocator_size_of_meta() noexcept
{
	return sizeof(allocator *) + sizeof(logger *) + sizeof(fit_mode) + sizeof(std::mutex) + sizeof(block_size_t) + sizeof(block_pointer_t); // root
}

inline allocator::block_size_t allocator_red_black_tree::get_free_block_size_of_meta() noexcept
{
	return sizeof(block_pointer_t) * 5 + sizeof(byte_occupied_color); // back* forward* parent* left* right* + 1 byte
}

inline allocator::block_size_t allocator_red_black_tree::get_occupied_block_size_of_meta() noexcept
{
	return sizeof(block_pointer_t) * 3 + sizeof(byte_occupied_color); // back* forward* parent* + 1 byte
}

inline size_t allocator_red_black_tree::get_size_block(void* current_block, void* trusted_memory) noexcept
{
	if(get_forward_ptr(current_block) == nullptr)
	{
		return reinterpret_cast<unsigned char*>(trusted_memory) + get_allocator_size_of_meta() + get_size_full(trusted_memory) - reinterpret_cast<unsigned char*>(current_block) - get_occupied_block_size_of_meta();
	}
	return reinterpret_cast<unsigned char*>(get_forward_ptr(current_block)) - reinterpret_cast<unsigned char *>(current_block) - get_occupied_block_size_of_meta();
}


size_t allocator_red_black_tree::get_free_size() const noexcept
{
	size_t res = 0;

	for (auto it = begin_iter(), end= end_iter(); it != end; ++it)
	{
		if (!it.is_occup()) res += it.size();
	}
	return res;
}




void allocator_red_black_tree::remove_from_rb_tree(void *current_block) noexcept
{
	void* parent;
	bool need_rebalance = false;

	// Если удаляемый узел не имеет потомков
	if(get_right_ptr(current_block) == nullptr && get_left_ptr(current_block) == nullptr)
	{
		parent = get_parent(current_block);
		update_parent_ptr(current_block, nullptr);
		need_rebalance = get_byte_occupied_color(current_block)._color == color::BLACK;
	}
	else if(get_right_ptr(current_block) == nullptr || get_left_ptr(current_block) == nullptr)
	{
		void* button_node = get_right_ptr(current_block) != nullptr  ? get_right_ptr(current_block) : get_left_ptr(current_block);

		get_byte_occupied_color(button_node)._color = color::BLACK;

		update_parent_ptr(current_block, button_node);
		get_parent(button_node) = get_parent(current_block);

	}
	else
	{
		void* change_node = get_left_ptr(current_block);

		// Находим наибольший узел в левом поддереве
		while(get_right_ptr(change_node) != nullptr)
		{
			change_node = get_right_ptr(change_node);
		}

		need_rebalance = get_left_ptr(change_node) == nullptr && get_byte_occupied_color(change_node)._color == color::BLACK;

		parent = get_parent(change_node);

		if(get_byte_occupied_color(change_node)._color == color::BLACK && get_left_ptr(change_node) != nullptr)
		{
			get_byte_occupied_color(get_left_ptr(change_node))._color = color::BLACK;
		}

		update_parent_ptr(current_block, change_node);
		get_right_ptr(change_node) = get_right_ptr(current_block);
		get_parent(get_right_ptr(change_node)) = change_node;

		if(get_parent(change_node) != current_block)
		{
			update_parent_ptr(change_node, get_left_ptr(change_node));
			if(get_left_ptr(change_node) != nullptr)
			{
				get_parent(get_left_ptr(change_node)) = get_parent(change_node);
			}
			get_left_ptr(change_node) = get_left_ptr(current_block);
			get_parent(get_left_ptr(change_node)) = change_node;
		}
		else
		{
			parent = change_node;
		}

		get_byte_occupied_color(change_node)._color = get_byte_occupied_color(current_block)._color;
		get_parent(change_node) = get_parent(current_block);

	}

	if(need_rebalance)
	{
		rebalance(parent);
	}
}

void allocator_red_black_tree::update_parent_ptr(void* current_block, void* new_parent) noexcept
{
	if(get_parent(current_block) == nullptr)
	{
		*get_first_block(_trusted_memory) = new_parent;
	}
	else
	{
		if(current_block == get_left_ptr(get_parent(current_block))) // is left child
		{
			get_left_ptr(get_parent(current_block)) = new_parent;
		}
		else
		{
			get_right_ptr(get_parent(current_block)) = new_parent;
		}
	}
}

void allocator_red_black_tree::rebalance(void* parent, void* deleted)
{
	if(parent == nullptr)
	{
		// Если удаленный узел не nullptr, перекрашиваем его в черный цвет
		if(deleted != nullptr)
		{
			get_byte_occupied_color(deleted)._color = color::BLACK;
		}
	}
	else
	{
		bool is_left = deleted == get_left_ptr(parent);

		void* brother = is_left ? get_right_ptr(parent) : get_left_ptr(parent);

		if(get_byte_occupied_color(brother)._color == color::RED)
		{
			is_left ? small_left_rotate(parent) : small_right_rotate(parent);

			get_byte_occupied_color(parent)._color = color::RED;
			get_byte_occupied_color(brother)._color = color::BLACK;

			rebalance(parent, deleted);
		}
		else
		{
			void* far_cousin = is_left ? get_right_ptr(brother) : get_left_ptr(brother);
			void* near_cousin = is_left ? get_left_ptr(brother) : get_right_ptr(brother);

			if(far_cousin != nullptr && get_byte_occupied_color(far_cousin)._color == color::RED)
			{
				is_left ? small_left_rotate(parent) : small_right_rotate(parent);

				get_byte_occupied_color(brother)._color = get_byte_occupied_color(parent)._color;
				get_byte_occupied_color(parent)._color = color::BLACK;
				get_byte_occupied_color(far_cousin)._color = color::BLACK;
			}
			else if(near_cousin != nullptr && get_byte_occupied_color(near_cousin)._color == color::RED)
			{
				is_left ? big_left_rotate(parent) : big_right_rotate(parent);

				get_byte_occupied_color(near_cousin)._color = get_byte_occupied_color(parent)._color;
				get_byte_occupied_color(parent)._color = color::BLACK;
			}
			else
			{
				get_byte_occupied_color(brother)._color = color::RED;

				if(get_byte_occupied_color(parent)._color == color::RED)
				{
					get_byte_occupied_color(parent)._color = color::BLACK;
				}
				else
				{
					rebalance(get_parent(parent), parent);
				}
			}
		}
	}
}


void allocator_red_black_tree::small_right_rotate(void* root) noexcept
{
	if(get_left_ptr(root) != nullptr )
	{
		void* left_son = get_left_ptr(root);
		update_parent_ptr(root, left_son);
		get_parent(left_son) = get_parent(root);
		void* right_son_of_left_son_of_joint = get_right_ptr(left_son);
		get_right_ptr(left_son) = root;
		get_parent(root) = left_son;
		get_left_ptr(root) = right_son_of_left_son_of_joint;
		if(right_son_of_left_son_of_joint != nullptr)
		{
			get_parent(right_son_of_left_son_of_joint) = root;
		}

	}
}

void allocator_red_black_tree::small_left_rotate(void* root) noexcept
{
	if(get_right_ptr(root) != nullptr)
	{
		void* right_son = get_right_ptr(root);
		update_parent_ptr(root, right_son);
		get_parent(right_son) = get_parent(root);
		void* left_son_of_right_son_of_joint = get_left_ptr(right_son);
		get_left_ptr(right_son) = root;
		get_parent(root) = right_son;
		get_right_ptr(root) = left_son_of_right_son_of_joint;
		if(left_son_of_right_son_of_joint != nullptr)
		{
			get_parent(left_son_of_right_son_of_joint) = root;
		}
	}
}

void allocator_red_black_tree::big_right_rotate(void* root) noexcept
{
	if(get_left_ptr(root) != nullptr && get_right_ptr(get_left_ptr(root)) != nullptr)
	{
		void* node = get_left_ptr(root);

		small_left_rotate(node);
		small_right_rotate(root);
	}
}

void allocator_red_black_tree::big_left_rotate(void *root) noexcept
{
	if(get_right_ptr(root) != nullptr && get_left_ptr(get_right_ptr(root)) != nullptr)
	{
		void* node = get_right_ptr(root);

		small_right_rotate(node);
		small_left_rotate(root);
	}
}

void allocator_red_black_tree::insert_rb_tree(void* current_block) noexcept
{
	void* root = *get_first_block(_trusted_memory);
	void* parent = nullptr;

	// Поиск места для вставки нового узла
	while(root != nullptr)
	{
		if(get_size_block(current_block, _trusted_memory) < get_size_block(root, _trusted_memory))
		{
			parent = root;
			root = get_left_ptr(root);
		}
		else
		{
			parent = root;
			root = get_right_ptr(root);
		}
	}

	// Установка указателей на родителя и потомков для нового узла
	get_parent(current_block) = parent;
	get_left_ptr(current_block) = nullptr;
	get_right_ptr(current_block) = nullptr;

	get_byte_occupied_color(current_block).is_occupied = false;
	get_byte_occupied_color(current_block)._color = color::RED;

	// Если новый узел является корнем дерева
	if(parent == nullptr)
	{
		*get_first_block(_trusted_memory) = current_block;
	}
	else
	{
		// Установка нового узла как правого или левого потомка родителя
		if(get_size_block(current_block, _trusted_memory) >= get_size_block(parent, _trusted_memory))
		{
			get_right_ptr(parent) = current_block;
		}
		else
		{
			get_left_ptr(parent) = current_block;
		}
	}

	// Если новый узел является корнем дерева, перекрашиваем его в черный цвет и выходим
	if(parent == nullptr)
	{
		get_byte_occupied_color(current_block)._color = color::BLACK;
		return;
	}

	bool is_left = current_block == get_left_ptr(parent);

	// Восстановление свойств красно-черного дерева
	while(true)
	{
		if(get_byte_occupied_color(parent)._color == color::BLACK)
		{
			break;
		}
		if(get_byte_occupied_color(parent)._color == color::RED)
		{
			void* grand = get_parent(parent);

			if(get_right_ptr(grand) == parent)
			{
				if((get_left_ptr(grand) == nullptr || get_byte_occupied_color(get_left_ptr(grand))._color == color::BLACK) && !is_left)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					small_left_rotate(grand);
					get_byte_occupied_color(get_parent(grand))._color = color::BLACK;
					break;
				}
				else if((get_left_ptr(grand) == nullptr || get_byte_occupied_color(get_left_ptr(grand))._color == color::BLACK) && is_left)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					big_left_rotate(grand);
					get_byte_occupied_color(get_parent(grand))._color = color::BLACK;
					break;
				}
				else if(get_byte_occupied_color(get_left_ptr(grand))._color == color::RED)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					get_byte_occupied_color(get_left_ptr(grand))._color = color::BLACK;
					get_byte_occupied_color(get_right_ptr(grand))._color = color::BLACK;
					current_block = grand;
				}
			}
			else if(get_left_ptr(grand) == parent)
			{
				if((get_right_ptr(grand) == nullptr || get_byte_occupied_color(get_right_ptr(grand))._color == color::BLACK) && is_left)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					small_right_rotate(grand);
					get_byte_occupied_color(get_parent(grand))._color = color::BLACK;
					break;
				}
				else if((get_right_ptr(grand) == nullptr || get_byte_occupied_color(get_right_ptr(grand))._color == color::BLACK) && !is_left)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					big_right_rotate(grand);
					get_byte_occupied_color(get_parent(grand))._color = color::BLACK;
					break;
				}
				else if(get_byte_occupied_color(get_right_ptr(grand))._color == color::RED)
				{
					get_byte_occupied_color(grand)._color = color::RED;
					get_byte_occupied_color(get_left_ptr(grand))._color = color::BLACK;
					get_byte_occupied_color(get_right_ptr(grand))._color = color::BLACK;
					current_block = grand;
				}
			}
		}

		// Если текущий узел стал корнем, перекрашиваем его в черный цвет и выходим
		if(get_parent(current_block) == nullptr)
		{
			get_byte_occupied_color(current_block)._color = color::BLACK;
			break;
		}

		parent = get_parent(current_block);
		is_left = current_block == get_left_ptr(parent);
	}
}


std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info() const noexcept
{
	std::vector<allocator_test_utils::block_info> result;

	for(auto it = begin_iter(), end = end_iter(); it != end; ++it)
	{
		result.push_back({it.size(), it.is_occup()});
	}

	return result;

}

std::string allocator_red_black_tree::get_blocks_info_to_string(const std::vector<allocator_test_utils::block_info>& vector) const noexcept
{
	std::ostringstream str;
	for(auto& it : vector)
	{
		if(it.is_block_occupied) str << "<occup>";
		else str << "<avail>";

		str << "<" + std::to_string(it.block_size) + "> | ";
	}
	return str.str();
}

inline void allocator_red_black_tree::set_fit_mode(
		allocator_with_fit_mode::fit_mode mode)
{
	std::lock_guard lock(get_mutex());
	auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
	byte_ptr += sizeof(logger*) + sizeof(allocator*);
	*reinterpret_cast<allocator_with_fit_mode::fit_mode*>(byte_ptr) = mode;
}

std::string allocator_red_black_tree::get_dump(char* at, size_t size)
{
	std::string result;
	for(size_t i = 0; i < size; ++i)
		result += std::to_string(static_cast<int>(at[i])) + " ";
	return result;
}

inline std::string allocator_red_black_tree::get_typename() const noexcept
{
	return "allocator_red_black_tree";
}



allocator_red_black_tree::iterator::iterator() : _ptr(nullptr), trusted_mem(nullptr) {}

allocator_red_black_tree::iterator::iterator(void *trust)
{
	_ptr = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(trust) + get_allocator_size_of_meta());
	trusted_mem = trust;
}

allocator_red_black_tree::iterator allocator_red_black_tree::begin_iter() const noexcept
{
	return {_trusted_memory};
}

allocator_red_black_tree::iterator allocator_red_black_tree::end_iter() const noexcept
{
	return {};
}

bool allocator_red_black_tree::iterator::operator==(const allocator_red_black_tree::iterator &oth) const noexcept
{
	return _ptr == oth._ptr;
}

bool allocator_red_black_tree::iterator::operator!=(const allocator_red_black_tree::iterator &oth) const noexcept
{
	return !(*this == oth);
}

allocator_red_black_tree::iterator& allocator_red_black_tree::iterator::operator++() noexcept
{
	_ptr = get_forward_ptr(_ptr);
	return *this;
}

allocator_red_black_tree::iterator allocator_red_black_tree::iterator::operator++(int) noexcept
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

size_t allocator_red_black_tree::iterator::size()
{
	return get_size_block(_ptr, trusted_mem);
}

void* allocator_red_black_tree::iterator::get_ptr_free_block() const noexcept
{
	return _ptr;
}

bool allocator_red_black_tree::iterator::is_occup() const noexcept
{
	return get_byte_occupied_color(_ptr).is_occupied;
}