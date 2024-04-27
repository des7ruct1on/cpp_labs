#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags() {
    debug_with_guard(get_typename() + " [START] " + "destructor");
    logger * _logger = get_logger();
    allocator::destruct(&get_mutex());
    deallocate_with_guard(_trusted_memory);
    if (_logger != nullptr) {
        _logger->debug(get_typename() + " [END] " + "destructor");
    }
}

allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&other) noexcept {
    logger * log = other.get_logger();
    if (log != nullptr) {
        log->debug(get_typename() + " [START] " + "move constructor\n");
    }
    if (_trusted_memory != nullptr) {
        deallocate_with_guard(_trusted_memory);
    }
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;

    if (log != nullptr) log->debug(get_typename() + " [END] " + "move constructor\n");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&other) noexcept {
    logger * log = other.get_logger();
    if (log != nullptr) {
        log->debug(get_typename() + " [START] " + "move operator\n");
    }
    if (this == &other) {
        debug_with_guard(get_typename() + " [END] " + "move operator\n");
        return *this;
    }

    if (_trusted_memory != nullptr) {
        deallocate_with_guard(_trusted_memory);
    }
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;

    if (log != nullptr) {
        log->debug(get_typename() + " [END] " + "move operator\n");
    }
    return *this;
}

allocator_boundary_tags::allocator_boundary_tags(size_t space_size, allocator *parent_allocator, logger *_logger, allocator_with_fit_mode::fit_mode allocate_fit_mode) {

    if (_logger != nullptr) {
        _logger->debug(get_typename() + " [START] " + "constructor");
    }

    if (space_size < block_meta_size + sizeof(void*)) {
        std::string error = get_typename() + " [START] " + "can`t allocate, no space\n";
        if (_logger != nullptr) {
            _logger->error(error);
        }
        throw std::logic_error(error);
    }

    try {
        if (parent_allocator != nullptr) {
            _trusted_memory = parent_allocator->allocate(meta_size + space_size + block_meta_size, 1);
        } else {
            _trusted_memory = ::operator new(meta_size + space_size + block_meta_size);
        }
    } catch(const std::exception& e) {
        std::string error = get_typename() + " [START] " + "bad allocation detected\n";
        if (_logger != nullptr) {
            _logger->error(error);
        }
        throw std::logic_error(error);
    }

    unsigned char * memory_ptr = reinterpret_cast<unsigned char *>(_trusted_memory);

    *reinterpret_cast<size_t*>(memory_ptr) = space_size;
    memory_ptr += sizeof(size_t);

    *reinterpret_cast<allocator**>(memory_ptr) = parent_allocator;
    memory_ptr += sizeof(allocator*);

    *reinterpret_cast<logger**>(memory_ptr) = _logger;
    memory_ptr += sizeof(logger*);

    *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(memory_ptr) = allocate_fit_mode;
    memory_ptr += sizeof(allocator_with_fit_mode::fit_mode);

    allocator::construct(reinterpret_cast<std::mutex *>(memory_ptr));
    memory_ptr += sizeof(std::mutex);

    *reinterpret_cast<void**>(memory_ptr) = nullptr; 
    memory_ptr += sizeof(void*);

    if (_logger != nullptr) {
        _logger->debug(get_typename() + " [END] " + "constructor");
    }
}

[[nodiscard]] void *allocator_boundary_tags::allocate(size_t value_size, size_t values_count) {
    std::lock_guard<std::mutex> mutex_guard(get_mutex());
    debug_with_guard(get_typename() + " [START] " + "allocation");

    auto need_size = value_size * values_count;

    if (need_size < sizeof(void*)) {
        need_size = sizeof(void*);
        warning_with_guard(get_typename() + " size of needed block has changed\n");
    }

    allocator_with_fit_mode::fit_mode fit_mode = get_fit_mode();

    void * prev_filled_block = nullptr;
    void * cur_filled_block = get_first_filled_block();
    void * need_block = nullptr;
    size_t prev_size = 0;

    void * need_prev_ptr = nullptr;
    void * need_next_ptr = nullptr;

    size_t available_block_size = get_size_memory();
    void * cur_avail_block;

    // all memory is free
    if (cur_filled_block == nullptr && available_block_size >= need_size + block_meta_size) {
        need_block = get_first_block();
    }

    while (cur_filled_block != nullptr) {

        if (prev_filled_block == nullptr) {
            cur_avail_block = get_first_block();

        } else {
            cur_avail_block = reinterpret_cast<unsigned char *>(prev_filled_block) + block_meta_size + get_size_block(prev_filled_block);
        
        }
        if (cur_avail_block != cur_filled_block) {
            available_block_size = reinterpret_cast<unsigned char*>(cur_filled_block) - reinterpret_cast<unsigned char *>(cur_avail_block);
            if (available_block_size >= need_size + block_meta_size) {
                if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit) {
                    need_block = cur_avail_block;
                    prev_size = available_block_size;
                    need_prev_ptr = prev_filled_block;
                    need_next_ptr = cur_filled_block;
                    break;
                } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) {
                    if (available_block_size < prev_size || prev_size == 0)
                    {
                        need_block = cur_avail_block;
                        prev_size = available_block_size;
                        need_prev_ptr = prev_filled_block;
                        need_next_ptr = cur_filled_block;
                    }
                } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit) {
                    if (available_block_size > prev_size) {
                        need_block = cur_avail_block;
                        prev_size = available_block_size;
                        need_prev_ptr = prev_filled_block;
                        need_next_ptr = cur_filled_block;
                    }
                }
            }
        }
        prev_filled_block = cur_filled_block;
        cur_filled_block = get_next_block(cur_filled_block);
    }
    if (cur_filled_block == nullptr && prev_filled_block != nullptr && prev_filled_block != get_end_ptr()) {
        cur_avail_block = reinterpret_cast<unsigned char *>(prev_filled_block) + block_meta_size + get_size_block(prev_filled_block);
        available_block_size = reinterpret_cast<unsigned char*>(get_end_ptr()) - reinterpret_cast<unsigned char *>(cur_avail_block);
        if (available_block_size >= need_size + block_meta_size) {
            if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit && need_block == nullptr)
            {
                need_block = cur_avail_block;
                prev_size = available_block_size;
                need_prev_ptr = prev_filled_block;
                need_next_ptr = cur_filled_block;
            } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) {
                if (available_block_size < prev_size || prev_size == 0) {
                    need_block = cur_avail_block;
                    prev_size = available_block_size;
                    need_prev_ptr = prev_filled_block;
                    need_next_ptr = cur_filled_block;
                }
            } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit) {
                if (available_block_size > prev_size) {
                    need_block = cur_avail_block;
                    prev_size = available_block_size;
                    need_prev_ptr = prev_filled_block;
                    need_next_ptr = cur_filled_block;
                }
            }
        }
    }

    if (need_block == nullptr)  {   
        error_with_guard(get_typename() + " no space to allocate\n");
        throw std::bad_alloc();

    }

    if (cur_filled_block == nullptr && prev_filled_block == nullptr) {
        prev_size = get_size_memory();

    }

    size_t blocks_sizes_difference = prev_size - (need_size + block_meta_size);
    if (blocks_sizes_difference > 0 && blocks_sizes_difference < block_meta_size) {
        need_size += blocks_sizes_difference;
        warning_with_guard(get_typename() + " size of needed block has changed\n");
    }
    
    concat_block(need_prev_ptr, need_block);
    concat_block(need_block, need_next_ptr);
    
    *reinterpret_cast<size_t*>(need_block) = need_size;
    *reinterpret_cast<allocator**>(reinterpret_cast<unsigned char *>(need_block) + sizeof(size_t)) = this;

    void * res = reinterpret_cast<unsigned char *>(need_block) + block_meta_size;

    information_with_guard(get_typename() + "   -> Available memory: " + std::to_string(get_available_memory()));

    debug_with_guard(get_blocks_info(get_blocks_info()));

    debug_with_guard(get_typename() + " [END] " + "allocation");
    return res;
}

// вывод текущего состояния блока
std::string allocator_boundary_tags::get_block_info(void * block) const noexcept {
    unsigned char * bytes = reinterpret_cast<unsigned char *>(block);
    size_t size = get_size_block(bytes - sizeof(size_t) - sizeof(allocator*) - 2 * sizeof(void*));
    std::string array = "";

    for (block_size_t i = 0; i < size; ++i) {
        array += std::to_string(*bytes);
        bytes += sizeof(unsigned char);
        array += ' ';

    }
    return array;
}

void allocator_boundary_tags::deallocate(void *at) {
    debug_with_guard(get_typename() + " [START] " + " deallocation\n");

    std::string block_info_array = get_block_info(at);
    debug_with_guard(get_typename() + " " + block_info_array);

    auto meta = 2 * sizeof(void*) + sizeof(size_t) + sizeof(allocator*);

    unsigned char * block = reinterpret_cast<unsigned char *>(at) - meta;
    if (this != get_allocator_from_block(block)) {
        std::string error = " block hasnt made by this allocator\n";
        error_with_guard(get_typename() + error);
        throw std::logic_error(error);

    }

    void * prev_block = get_prev_block(block);
    void * next_block =  get_next_block(block);

    concat_block(prev_block, next_block);

    clear_block(block);

    information_with_guard(get_typename() + " Available memory: " + std::to_string(get_available_memory()));

    debug_with_guard(get_blocks_info(get_blocks_info()));

    debug_with_guard(get_typename() + " [END] " + " deallocation\n");
}

size_t allocator_boundary_tags::get_available_memory() const noexcept {
    void * cur = get_first_filled_block();
    if (cur == nullptr) {
        return get_size_memory();
    }
    void* next = nullptr;

    size_t memory_occupied = 0;

    auto meta_size = sizeof(size_t) + sizeof(allocator*) + 2 * sizeof(void*);

    while (cur != nullptr) {
        memory_occupied += get_size_block(cur) + meta_size;
        cur = get_next_block(cur);

    }
    size_t res = get_size_memory() - memory_occupied;
    return res;
}

void allocator_boundary_tags::clear_block(void * block) const noexcept {
    unsigned char* block_ptr = reinterpret_cast<unsigned char *>(block);
    size_t* block_size = reinterpret_cast<size_t*>(block);
    block_size = nullptr;

    block_ptr += sizeof(size_t);
    allocator** alc = reinterpret_cast<allocator**>(block_ptr);
    alc = nullptr;

    block_ptr += sizeof(allocator*);
    void** prev = reinterpret_cast<void**>(block_ptr);
    prev = nullptr;

    block_ptr += sizeof(void*);
    void** next = reinterpret_cast<void**>(block_ptr);
    next = nullptr;
}

inline void allocator_boundary_tags::set_fit_mode(allocator_with_fit_mode::fit_mode mode) {
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator*) + sizeof(logger*) + sizeof(size_t)) = mode;
}

inline allocator *allocator_boundary_tags::get_allocator() const {
    return *reinterpret_cast<allocator**>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t));
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept {
    std::vector<allocator_test_utils::block_info> info;
    void* cur = get_first_filled_block();
    if (cur == nullptr) {
        allocator_test_utils::block_info block;
        block.block_size = get_size_memory();
        block.is_block_occupied = false;
        info.push_back(block);
        return info;
    }

    void * prev = nullptr;

    auto meta_size = sizeof(size_t) + sizeof(allocator*) + 2 * sizeof(void*);

    while (cur != nullptr) {
        if ((prev == nullptr && cur != get_first_block()) || (prev != nullptr && (reinterpret_cast<unsigned char *>(prev) + meta_size + get_size_block(prev) != cur))) {
            size_t size;
            if (prev == nullptr) {
                size = reinterpret_cast<unsigned char *>(cur) - (reinterpret_cast<unsigned char*>(get_first_block()));
            } else {
                size = reinterpret_cast<unsigned char *>(cur) - (reinterpret_cast<unsigned char*>(prev) + meta_size + get_size_block(prev));
            }
            allocator_test_utils::block_info avail_block;
            avail_block.block_size = size;
            avail_block.is_block_occupied = false;
            info.push_back(avail_block);
        }
        allocator_test_utils::block_info occup_block;
        occup_block.block_size = get_size_block(cur);
        occup_block.is_block_occupied = true;
        info.push_back(occup_block);
        prev = cur;
        cur = get_next_block(cur);
    }
    if (prev != get_end_ptr() && prev != nullptr) {
        size_t size = reinterpret_cast<unsigned char *>(get_end_ptr()) - (reinterpret_cast<unsigned char*>(prev) + meta_size + get_size_block(prev));
        allocator_test_utils::block_info avail_block;
        avail_block.block_size = size;
        avail_block.is_block_occupied = false;
        if (size != 0) {
            info.push_back(avail_block);
        }
    }
    return info;
}

std::string allocator_boundary_tags::get_blocks_info(std::vector<allocator_test_utils::block_info> info) const noexcept
{
    std::string blocks = "Info from blocks:\n";
    int k = 1;
    for (auto &block : info) {
        std::string small_info = "\t" + std::to_string(k++) + ". <" + block_status(block.is_block_occupied) + "> <" + std::to_string(block.block_size) + ">\n";
        blocks += small_info;
        
    }
    return blocks;
}

std::string allocator_boundary_tags::block_status(bool state) const noexcept {
    if (state) {
        return "occup";
    }
    return "avail";
}

inline logger *allocator_boundary_tags::get_logger() const {
    return *reinterpret_cast<logger**>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*));
}

inline std::string allocator_boundary_tags::get_typename() const noexcept {
    return "[allocator_boundary_tags]";
}

inline allocator_with_fit_mode::fit_mode allocator_boundary_tags::get_fit_mode() {
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(reinterpret_cast<unsigned char *>(_trusted_memory)  + sizeof(size_t) + sizeof(allocator*) + sizeof(logger*));
}

void * allocator_boundary_tags::get_first_filled_block() const noexcept {
    return *reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*) + 
        sizeof(logger*) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex));
}

void * allocator_boundary_tags::get_first_block() const noexcept {
    return reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*) + 
        sizeof(logger*) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + sizeof(void*);
}

size_t allocator_boundary_tags::get_size_block(void * block) const noexcept {
    return *reinterpret_cast<size_t*>(block);
}

void * allocator_boundary_tags::get_end_ptr() const noexcept {
    return reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*) + sizeof(logger*) + 
        sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + sizeof(void*) + get_size_memory();
}

allocator * allocator_boundary_tags::get_allocator_from_block(void* block) const noexcept {
    return *reinterpret_cast<allocator**>(reinterpret_cast<unsigned char *>(block) + sizeof(size_t));
}

void * allocator_boundary_tags::get_prev_block(void* block) const noexcept {
    return *reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(block) + sizeof(allocator*) + sizeof(size_t));
}

void * allocator_boundary_tags::get_next_block(void* block) const noexcept {
    return *reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(block) + sizeof(allocator*) + sizeof(size_t) + sizeof(void*));
}

void allocator_boundary_tags::concat_block(void* prev, void* next) noexcept {
    if (prev != nullptr) {
        void ** new_next = reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(prev) + sizeof(size_t) + sizeof(allocator*) + sizeof(void*));
        *new_next = next;
    }else {
        set_first_filled_block(next);
    }
    if (next != nullptr) {
        void ** new_prev = reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(next) + sizeof(size_t) + sizeof(allocator*));
        *new_prev = prev;
    }
}

void allocator_boundary_tags::set_first_filled_block(void* block) const noexcept {
    *reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*) + sizeof(logger*) +
         sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex)) = block;
}

std::mutex & allocator_boundary_tags::get_mutex() const noexcept {
    return *reinterpret_cast<std::mutex*>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(size_t) + sizeof(allocator*) + 
        sizeof(logger*) + sizeof(allocator_with_fit_mode::fit_mode));
}

size_t allocator_boundary_tags::get_size_memory() const noexcept {
    return *reinterpret_cast<size_t*>(_trusted_memory);
}