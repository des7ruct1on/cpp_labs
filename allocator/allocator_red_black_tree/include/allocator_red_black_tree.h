#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <sstream>

class allocator_red_black_tree final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

	enum class color : unsigned char 
	{
		RED,
		BLACK
	};

	struct byte_occupied_color
	{
		bool is_occupied : 4;
		color _color : 4;
	};

    void *_trusted_memory;

public:

    ~allocator_red_black_tree() override;

    allocator_red_black_tree(
        allocator_red_black_tree const &other) = delete;

    allocator_red_black_tree &operator=(
        allocator_red_black_tree const &other) = delete;

    allocator_red_black_tree(
        allocator_red_black_tree &&other) noexcept;

    allocator_red_black_tree &operator=(
        allocator_red_black_tree &&other) noexcept;

public:

    explicit allocator_red_black_tree(
        size_t space_size,
        allocator *parent_allocator = nullptr,
        logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:

    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;

    void deallocate(
        void *at) override;

public:

    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

public:

	std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:

    inline allocator *get_allocator() const override;

private:

    inline logger *get_logger() const override;

private:

    inline std::string get_typename() const noexcept override;


private:

	static inline allocator::block_size_t get_allocator_size_of_meta() noexcept;

	static inline allocator::block_size_t get_occupied_block_size_of_meta() noexcept;

	static inline allocator::block_size_t get_free_block_size_of_meta() noexcept;

	static inline size_t get_size_block(void* current_block, void* trusted_memory) noexcept;

	inline allocator_with_fit_mode::fit_mode& get_fit_mode() const noexcept;

	inline std::mutex& get_mutex() const noexcept;

	static inline void** get_first_block(void* trusted_mem) noexcept;

	static inline size_t get_size_full(void * trusted_m) noexcept;

	static byte_occupied_color& get_byte_occupied_color(void* current_block) noexcept;

	static void*& get_back_ptr(void* current_block) noexcept;

	static void*& get_forward_ptr(void* current_block) noexcept;

	static void*& get_parent(void* current_block) noexcept;

	void update_parent_ptr(void* current_block, void* new_parent) noexcept;

	static void*& get_left_ptr(void* current_block) noexcept;

	static void*& get_right_ptr(void* current_block) noexcept;

	void* get_first_suitable(size_t size) const noexcept;

	void* get_worst_suitable(size_t size) const noexcept;

	void* get_best_suitable(size_t size) const noexcept;

	void remove_from_rb_tree(void* current_block) noexcept;

	void insert_rb_tree(void* current_block) noexcept;

	void rebalance(void* parent, void* deleted = nullptr);

	void small_right_rotate(void* root) noexcept;

	void small_left_rotate(void* root) noexcept;

	void big_right_rotate(void* root) noexcept;

	void big_left_rotate(void* root) noexcept;

	class iterator
	{
		void* trusted_mem ;
		void* _ptr;

		public:

		iterator();

		iterator(void* ptr);

		bool operator==(const iterator& oth) const noexcept;

		bool operator!=(const iterator& oth) const noexcept;

		iterator& operator++() noexcept;

		iterator operator++(int) noexcept;

		size_t size();

		void* get_ptr_free_block() const noexcept;

		bool is_occup() const noexcept;

	};

	friend class iterator;

	iterator begin_iter() const noexcept;

	iterator end_iter() const noexcept;


	std::string get_blocks_info_to_string(const std::vector<allocator_test_utils::block_info>& vector) const noexcept;

	size_t get_free_size() const noexcept;

	static std::string get_dump(char* at, size_t size);
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H