#include <sstream>

#include "../include/allocator_buddies_system.h"

allocator_buddies_system::~allocator_buddies_system()
{
	debug_with_guard(get_typename() + " [Destructor] [Start]");
	deallocate_with_guard(_trusted_memory);
	get_mutex().~mutex();
	left_bytes = 0;
    debug_with_guard(get_typename() + " [Destructor] [Finish]");
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system &&other) noexcept : _trusted_memory(other._trusted_memory), left_bytes(other.left_bytes), meta_block_power_(other.meta_block_power_)
{
	debug_with_guard(get_typename() + "[Move constructor] [Start]");
	other._trusted_memory = nullptr;
    debug_with_guard(get_typename() + "[Move constructor] [Finish]");
}

allocator_buddies_system &allocator_buddies_system::operator=(allocator_buddies_system &&other) noexcept
{
    debug_with_guard(get_typename() + "[Start assignment operator] [Start]");
	if (this != &other) {
		get_mutex().~mutex();
		deallocate_with_guard(_trusted_memory);

		_trusted_memory = other._trusted_memory;
		left_bytes = other.left_bytes;
		meta_block_power_ = other.meta_block_power_;
		other._trusted_memory = nullptr;
	}
    debug_with_guard(get_typename() + "[Start assignment operator] [Finish]");
	return *this;
}

allocator_buddies_system::allocator_buddies_system(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode) : meta_block_power_(calculate_meta_block_power())
{
	if (space_size <= meta_block_power_) {
		throw std::logic_error("Can`t initialize allocator");
	}

	size_t allocator_size = (1 << space_size) + get_allocator_size_of_meta();
	try {
		_trusted_memory = parent_allocator == nullptr ? ::operator new(allocator_size) : parent_allocator->allocate(allocator_size, 1);
	} catch (std::bad_alloc const & ex) {
        // catch bad_alloc
		error_with_guard(get_typename() + " didnt allocate " + std::to_string(allocator_size) + " bytes of memory");
		throw ex;
	}
    debug_with_guard(get_typename() + " [Constructor] [Start]");
	auto ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

	*reinterpret_cast<allocator**>(ptr) = parent_allocator == nullptr ? this : parent_allocator;
	ptr += sizeof(allocator *);

	*reinterpret_cast<class logger**>(ptr) = logger;
	ptr += sizeof(class logger *);

	*reinterpret_cast<fit_mode*>(ptr) = allocate_fit_mode;
	ptr += sizeof(fit_mode);

	construct(reinterpret_cast<std::mutex *>(ptr));
	ptr += sizeof(std::mutex);

	*reinterpret_cast<unsigned char*>(ptr) = space_size;
	ptr += sizeof(unsigned char);

	reinterpret_cast<byte_for_occup_and_power_of_size*>(ptr)->occupied = false;
	reinterpret_cast<byte_for_occup_and_power_of_size*>(ptr)->size = space_size - meta_block_power_;
	left_bytes = 1 << space_size;

	debug_with_guard(get_typename() + " [Constructor] [Finish]");
}

[[nodiscard]] void *allocator_buddies_system::allocate(size_t value_size, size_t values_count)
{
	debug_with_guard(get_typename() + " [Allocation] [Start]");

	std::lock_guard<std::mutex> lock(get_mutex());

	size_t need_result_mem = value_size * values_count;
	size_t block_size = need_result_mem + get_block_occupied_size_of_meta();

	block_pointer_t cur_loaded_block;

	switch(get_fit_mode()) {
		case allocator_with_fit_mode::fit_mode::the_best_fit:
			cur_loaded_block = get_suitable(block_size, true); 
			break;
		case allocator_with_fit_mode::fit_mode::first_fit:
			cur_loaded_block = get_first_suitable(block_size);  
			break;
		case allocator_with_fit_mode::fit_mode::the_worst_fit:
			cur_loaded_block = get_suitable(block_size, false); 
			break;
	}

	if (cur_loaded_block == nullptr) {
		error_with_guard(get_typename() + "didnt find needed block for  " + std::to_string(block_size) + " bytes");
		throw std::bad_alloc();
	}

	// division on halfs
	while (get_size_block(cur_loaded_block) >= block_size * 2) {
		get_first_byte_of_block(&cur_loaded_block)->size--;
		auto l_brother = reinterpret_cast<byte_for_occup_and_power_of_size*>(get_brother(cur_loaded_block));
		l_brother->size = get_first_byte_of_block(&cur_loaded_block)->size;
		l_brother->occupied = false;
	}

	get_allocator_occupied_block(cur_loaded_block) = this;
	get_first_byte_of_block(&cur_loaded_block)->occupied = true;
	left_bytes -= get_size_block(cur_loaded_block);
	debug_with_guard(get_typename() + " [Allocation] [Finish]");
	information_with_guard(get_typename() + "current state of blocks: " + get_blocks_info_to_string(get_blocks_info()));

	return reinterpret_cast<unsigned char *>(cur_loaded_block) + get_block_occupied_size_of_meta();
}

void allocator_buddies_system::deallocate(void *at)
{
    // locking by mutex
	std::lock_guard<std::mutex> lock(get_mutex());
	debug_with_guard(get_typename() + " [Deallocate] [Start]");

	block_pointer_t block_ptr = at != _trusted_memory ? reinterpret_cast<unsigned char *>(at) - get_block_occupied_size_of_meta() : get_first_block_by_alloc(), brother = get_brother(block_ptr);
	if (get_allocator_occupied_block(block_ptr) != this && at != _trusted_memory) {
		error_with_guard("this block is not from this allocator");
		throw std::logic_error("This block is not from this allocator");
	}

	debug_with_guard("Block status before deallocation: " + get_dump(reinterpret_cast<char*>(at), get_size_block(block_ptr)));
	block_size_t size = get_size_block(block_ptr);
	get_first_byte_of_block(&block_ptr)->occupied = false;
	if (at != _trusted_memory) {
        left_bytes += get_size_block(block_ptr);
    }

	while (get_size_block(block_ptr) == get_size_block(brother) && !is_occupied(brother) && get_size_block(block_ptr) < get_allocator_size()) {
		auto merged_block = block_ptr < brother ? block_ptr : brother;
		get_first_byte_of_block(&merged_block)->size++;
		block_ptr = merged_block;
		brother = get_brother(block_ptr);
	}

	debug_with_guard(get_typename() + " [Deallocate] [Finish]");
	information_with_guard(get_typename() + " current state of blocks: " + get_blocks_info_to_string(get_blocks_info()));
}

inline allocator_buddies_system::byte_for_occup_and_power_of_size*& allocator_buddies_system::get_first_byte_of_block (block_pointer_t block) const noexcept
{
	return *reinterpret_cast<byte_for_occup_and_power_of_size**>(block);
}

inline allocator::block_size_t allocator_buddies_system::get_block_size_of_meta() const noexcept
{
	return sizeof(unsigned char);
}

inline allocator::block_size_t allocator_buddies_system::get_block_occupied_size_of_meta() const noexcept
{
	return sizeof(unsigned char) + sizeof(allocator*);
}

inline allocator::block_size_t allocator_buddies_system::calculate_meta_block_power() const noexcept
{
	block_size_t meta_block_size = get_block_occupied_size_of_meta(), power_of_two = 0;

	while (meta_block_size > 1) {
		meta_block_size >>= 1;
		++power_of_two;
	}

	return power_of_two;
}

inline allocator::block_size_t allocator_buddies_system::get_allocator_size_of_meta() const noexcept
{
	return sizeof(allocator *) + sizeof(logger *) + sizeof(fit_mode) + sizeof(std::mutex) + sizeof(unsigned char);
}

inline allocator *allocator_buddies_system::get_allocator() const
{
	return *reinterpret_cast<allocator**>(_trusted_memory);
}

inline logger *allocator_buddies_system::get_logger() const
{
	auto* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(allocator*);

	return *reinterpret_cast<logger**>(ptr);
}

inline allocator_with_fit_mode::fit_mode& allocator_buddies_system::get_fit_mode() const noexcept
{
	auto* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(allocator*) + sizeof(logger*);

	return *reinterpret_cast<fit_mode*>(ptr);
}

inline std::mutex & allocator_buddies_system::get_mutex() const noexcept
{
	auto* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(allocator*) + sizeof(logger*) + sizeof(fit_mode);

	return *reinterpret_cast<std::mutex*>(ptr);
}

inline size_t allocator_buddies_system::get_allocator_size() const noexcept
{
	auto* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += sizeof(allocator*) + sizeof(logger*) + sizeof(fit_mode) + sizeof(std::mutex);

	return 1 << *reinterpret_cast<unsigned char*>(ptr);
}

inline allocator::block_pointer_t allocator_buddies_system::get_first_block_by_alloc() const noexcept
{
	auto* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
	ptr += get_allocator_size_of_meta();

	return reinterpret_cast<block_pointer_t>(ptr);
}

bool allocator_buddies_system::is_occupied(block_pointer_t current_block) const noexcept
{
	return reinterpret_cast<byte_for_occup_and_power_of_size*>(current_block)->occupied;
}

inline allocator *&allocator_buddies_system::get_allocator_occupied_block(block_pointer_t cur_block) const noexcept
{
	return *reinterpret_cast<allocator**>(reinterpret_cast<unsigned char*>(cur_block) + sizeof(unsigned char));
}

inline allocator::block_size_t allocator_buddies_system::get_size_block(block_pointer_t cur_block) const noexcept
{
	unsigned char size = reinterpret_cast<byte_for_occup_and_power_of_size*>(cur_block)->size;
	return static_cast<block_size_t>(1u) << (size + meta_block_power_);
}

inline allocator::block_pointer_t& allocator_buddies_system::next_block (block_pointer_t cur_block) const noexcept
{
	return *reinterpret_cast<block_pointer_t*>(reinterpret_cast<unsigned char*>(cur_block) + get_size_block(cur_block));
}

inline allocator::block_pointer_t& allocator_buddies_system::end_alloc() const noexcept
{
	return *reinterpret_cast<block_pointer_t*>(reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_size_of_meta() + get_allocator_size());
}

allocator::block_pointer_t allocator_buddies_system::get_first_suitable(size_t need_size)  const noexcept
{
	block_pointer_t cur_block = get_first_block_by_alloc();

	while (cur_block != end_alloc()) {
		if (!is_occupied(cur_block) && get_size_block(cur_block) >= need_size) {
			return cur_block;
        }
		cur_block = &next_block(cur_block);
	}
	return nullptr;
}

allocator::block_pointer_t allocator_buddies_system::get_suitable(size_t need_size, bool is_best_fit) const noexcept
{
	allocator::block_pointer_t cur_block = get_first_block_by_alloc();
	allocator::block_pointer_t result = nullptr;
	allocator::block_size_t size = is_best_fit ? get_allocator_size() : 0;

	while (cur_block != end_alloc()) {
		if (!is_occupied(cur_block) && get_size_block(cur_block) >= need_size) {
			if (is_best_fit) {
				if (get_size_block(cur_block) <= size) {
					size = get_size_block(cur_block);
					result = cur_block;
				}
			} else {
				if (get_size_block(cur_block) >= size) {
					size = get_size_block(cur_block);
					result = cur_block;
				}
			}
		}
		cur_block = &next_block(cur_block);
	}
	return result;
}

inline allocator::block_pointer_t allocator_buddies_system::get_brother(block_pointer_t r_brother) const noexcept
{
	auto start = reinterpret_cast<unsigned char*>(r_brother) - (reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_size_of_meta());
	auto to_brother = start ^ get_size_block(r_brother);

	return reinterpret_cast<block_pointer_t>(reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_size_of_meta() + to_brother);
}

inline std::string allocator_buddies_system::get_typename() const noexcept
{
	return "allocator_buddies_system";
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
	std::vector<allocator_test_utils::block_info> result;
	block_pointer_t cur_block = get_first_block_by_alloc();

	while (cur_block != &end_alloc()) {
		result.push_back({get_size_block(cur_block), is_occupied(cur_block)});
		cur_block = &next_block(cur_block);
	}

	return result;
}

std::string allocator_buddies_system::get_blocks_info_to_string(const std::vector<allocator_test_utils::block_info>& vector) const noexcept
{
	std::ostringstream str;
	for(auto& it : vector) {
		if(it.is_block_occupied) {
            str << "<occup>";
        } else {
            str << "<avail>";
        }

		str << "<" + std::to_string(it.block_size) + "> | ";
	}
	return str.str();
}

inline void allocator_buddies_system::set_fit_mode(allocator_with_fit_mode::fit_mode mode) { 
    get_fit_mode() = mode; 
}

std::string allocator_buddies_system::get_dump(char* at, size_t size)
{
	std::string result;
	for(size_t i = 0; i < size; i++) {
		result += std::to_string(static_cast<int>(at[i])) + " ";
    }
	return result;
}