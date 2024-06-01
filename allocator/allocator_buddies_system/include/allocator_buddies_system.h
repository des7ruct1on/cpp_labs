#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>

class allocator_buddies_system final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    void *_trusted_memory;

	size_t left_bytes;

	allocator::block_size_t meta_block_power_;

	struct byte_for_occup_and_power_of_size
	{
		bool occupied : 1;
		unsigned char size : 7;
	};

public:

    ~allocator_buddies_system() override;

    allocator_buddies_system(allocator_buddies_system const &other) = delete;

    allocator_buddies_system &operator=(allocator_buddies_system const &other) = delete;

    allocator_buddies_system(allocator_buddies_system &&other) noexcept;

    allocator_buddies_system &operator=(allocator_buddies_system &&other) noexcept;

public:

    explicit allocator_buddies_system(
        size_t space_size_power_of_two,
        allocator *parent_allocator = nullptr,
        logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:

    [[nodiscard]] void *allocate(size_t value_size, size_t values_count) override;

    void deallocate(void *at) override;

public:

    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

private:

    inline allocator *get_allocator() const override;

public:

    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:

    inline logger *get_logger() const override;

private:

    inline std::string get_typename() const noexcept override;

private:

	inline byte_for_occup_and_power_of_size*& get_first_byte_of_block (block_pointer_t block) const noexcept;

	inline std::mutex &get_mutex() const noexcept;

	inline allocator_with_fit_mode::fit_mode& get_fit_mode() const noexcept;

	inline size_t get_allocator_size() const noexcept;

	inline block_pointer_t get_first_block_by_alloc() const noexcept;

	inline block_pointer_t get_brother(block_pointer_t r_brother) const noexcept;

	inline allocator::block_size_t get_block_size_of_meta() const noexcept;

	inline allocator::block_size_t get_block_occupied_size_of_meta() const noexcept;

	inline allocator::block_size_t get_allocator_size_of_meta() const noexcept;

	inline allocator *&get_allocator_occupied_block(block_pointer_t cur_block) const noexcept;

	inline block_size_t get_size_block(block_pointer_t cur_block) const noexcept;

	inline bool is_occupied(block_pointer_t current_block) const noexcept;

	inline block_size_t calculate_meta_block_power() const noexcept;

	inline block_pointer_t &next_block (block_pointer_t cur_block) const noexcept;

	inline block_pointer_t& end_alloc() const noexcept;

	block_pointer_t get_first_suitable(size_t need_size)  const noexcept;

	block_pointer_t get_suitable(size_t need_size, bool is_best_fit) const noexcept;

	std::string get_blocks_info_to_string(const std::vector<allocator_test_utils::block_info>& vector) const noexcept;

	std::string get_dump(char* at, size_t size);
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H