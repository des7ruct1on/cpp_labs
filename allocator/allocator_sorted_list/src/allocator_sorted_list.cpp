#include <not_implemented.h>
#include <cstring>
#include <mutex>

#include "../include/allocator_sorted_list.h"

allocator_sorted_list::~allocator_sorted_list() {
    std::string func = "destructor";
    debug_with_guard(get_typename() + " [START] " + func);
    logger * _logger = get_logger(); // get logger
    deallocate_with_guard(_trusted_memory); // deallocate memory
    if (_logger != nullptr) {
        _logger->debug(get_typename() + " [END] " + func);
    }
}

allocator_sorted_list::allocator_sorted_list(allocator_sorted_list &&other) noexcept {
    std::string func = "move constructor\n";
    logger * log = other.get_logger(); // get logger 
    if (log != nullptr) { // logging startinf function
        log->debug(get_typename() + " [START] " + func);
    }
    if (_trusted_memory != nullptr) { // if found memory in existing var
        deallocate_with_guard(_trusted_memory);
    }
    // replacing
    _trusted_memory = other._trusted_memory; 
    other._trusted_memory = nullptr;

    if (log != nullptr) {
        log->debug(get_typename() + " [END] " + func);
    }
}


allocator_sorted_list::allocator_sorted_list(size_t space_size, allocator *parent_allocator, logger *logger, allocator_with_fit_mode::fit_mode allocate_fit_mode) {
    
    std::string func = "constructor\n";

    if (logger != nullptr) {
        logger->debug(get_typename() + " [START] " + func);
    }
    auto meta_size = sizeof(size_t) + sizeof(allocator *) + sizeof(class logger *) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex);
    auto block_meta_size = sizeof(size_t) + sizeof(void*);

    if (space_size < block_meta_size + sizeof(void*)) { 
        std::string space_error = " wrong space_size, can`t allocate due a lack of size\n";
        if (logger != nullptr) { // logging error
            logger->error(get_typename() + space_error);
        }
        throw std::logic_error(space_error);
    }
    auto result_size = space_size + meta_size;
    
    try {
        if (parent_allocator != nullptr) { // if parent_alocator dont exist we have to allocate new
            _trusted_memory = parent_allocator->allocate(result_size, 1);
        } else {
            _trusted_memory = ::operator new(result_size); // if parent_allocator exists
        }
    } catch(std::bad_alloc const &ex) { 
        std::string error = " bad alloc catched while trying to allocate\n";
        if (logger != nullptr) {
            logger->error(get_typename() + error);
        }
        throw ex;
    }

    unsigned char* mem = reinterpret_cast<unsigned char*>(_trusted_memory);

    *reinterpret_cast<allocator**>(mem) = parent_allocator;
    mem += sizeof(allocator*);

    *reinterpret_cast<class logger**>(mem) = logger;
    mem += sizeof(class logger*);

    *reinterpret_cast<size_t*>(mem) = space_size;
    mem += sizeof(size_t);

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(mem) = allocate_fit_mode;
    mem += sizeof(allocator_with_fit_mode::fit_mode);

    *reinterpret_cast<void**>(mem) = mem + sizeof(void*) + sizeof(std::mutex);
    mem += sizeof(void*);

    allocator::construct(reinterpret_cast<std::mutex *>(mem));
    mem += sizeof(std::mutex);

    *reinterpret_cast<void**>(mem) = nullptr;
    mem += sizeof(void*);
    *reinterpret_cast<size_t*>(mem) = space_size;

    debug_with_guard(get_typename() + " [END] " + func);
}


[[nodiscard]] void *allocator_sorted_list::allocate(size_t value_size, size_t values_count) {
    std::lock_guard<std::mutex> mutex_guard(get_mutex());
    std::string func = "allocation\n";
    debug_with_guard(get_typename() + " [START] " + func);

    auto req_size = value_size * values_count; // need size
    if (req_size < sizeof(void*)) { // if need size < -> we have to change requested size to min
        req_size = sizeof(void*);
        warning_with_guard(get_typename() + " size has been changed to sizeof(void*)\n");
    }
    allocator_with_fit_mode::fit_mode fit_mode = get_fit_mode(); // getting fit mode

    auto _meta_size = sizeof(size_t) + sizeof(allocator*);
    auto res_size = _meta_size + req_size;

    size_t available_memory = 0; 
    // init ptrs for block and prev, next
    void* block = nullptr;
    void* prev = nullptr;
    void* next = nullptr;
    size_t prev_size = 0;

    void* current = get_first_available_block(); // get first free block
    void* previous = nullptr;

    while (current != nullptr) { // until cur exists
        size_t current_block_size = get_available_block_size(current);
        available_memory += current_block_size;
        if (current_block_size == 0) {
            break;
        }
        if (current_block_size >= res_size) {
            // cases of fittings
            if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit && block == nullptr) {
                block = current;
                prev = previous;
                next = get_available_block_next_block_address(current);
                prev_size = current_block_size;
            } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) {
                if (current_block_size < prev_size || prev_size == 0) {
                    block = current;
                    prev = previous;
                    next = get_available_block_next_block_address(current);
                    prev_size = current_block_size;
                }
            } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit) {
                if (current_block_size > prev_size) {
                    block = current;
                    prev = previous;
                    next = get_available_block_next_block_address(current);
                    prev_size = current_block_size;
                }
            }
        }
        previous = current;
        current = get_available_block_next_block_address(current);
    }
    if (block == nullptr) {
        error_with_guard(get_typename() + " block is empty due a lack of ability to allocate\n");
        throw std::bad_alloc();
    }
    // get difference between current block and requested size from params
    auto blocks_sizes_difference = get_available_block_size(block) - req_size; 

    // if block_difference less than meta size we have to change requested size
    if (blocks_sizes_difference > 0 && blocks_sizes_difference < _meta_size) { 
        warning_with_guard(get_typename() + " size has been changed\n");
        req_size += blocks_sizes_difference;
        res_size = req_size + _meta_size;
    } else if (blocks_sizes_difference > 0) { // if usual case 
        void** new_next = reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(block) + res_size);
        *reinterpret_cast<size_t*>(new_next + 1) = blocks_sizes_difference - sizeof(void*) - sizeof(size_t);

        // if right block is not free
        if (new_next + blocks_sizes_difference != next) {
            *new_next = next;  
        } else { // right block is free -> merge these blocks
            merge_blocks(0, new_next, next); 
        }
        // getting next avail block
        if (prev != nullptr) {
            void **next_block_adress = reinterpret_cast<void **>(prev);
            *next_block_adress = new_next;
        } else {
            set_first_available_block(new_next);
        }
    }

    // init prev_adress params to nullptr
    void** prev_id = reinterpret_cast<void**>(block);
    prev_id = nullptr;
    
    size_t* prev_size_ptr = reinterpret_cast<size_t*>(prev_id + 1);
    prev_size_ptr = nullptr;

    // init size
    size_t* size = reinterpret_cast<size_t*>(block);
    *size = req_size;

    *reinterpret_cast<allocator**>(size + 1) = this;

    // ptr of allocated block
    void* res = reinterpret_cast<unsigned char *>(block) + _meta_size;


    // get info of blocks
    std::vector<allocator_test_utils::block_info> blocks_info = get_blocks_info();
    print_blocks_info(blocks_info);

    information_with_guard(get_typename() + " available memory to use: " + std::to_string(available_memory));

    debug_with_guard(get_typename() + " [END] " + func);
    return res;
}

std::string allocator_sorted_list::get_block_info(void* block) const noexcept {
    unsigned char* bytes = reinterpret_cast<unsigned char *>(block); // ptr of bytes
    size_t size = get_occupied_block_size(bytes - sizeof(size_t) - sizeof(allocator*));

    std::string arr = ""; // creating bytes_info array
    for (block_size_t i = 0; i < size; ++i) {
        // adding bytes
        arr += std::to_string(*bytes);
        bytes += sizeof(unsigned char);
        arr += ' ';
    }
    return arr;
}

void allocator_sorted_list::deallocate(void* at) {
    std::lock_guard<std::mutex> mutex_guard(get_mutex());
    std::string func = "deallocation\n";

    debug_with_guard(get_typename() + " [START] " + func);
    // getting blocks status
    std::string block_info_array = get_block_info(at);

    // debuging blocks status
    debug_with_guard(get_typename() + "\n" + block_info_array);

    size_t _meta_size = sizeof(allocator*) + sizeof(size_t);
    size_t available_memory = 0;

    void* block = reinterpret_cast<unsigned char *>(at) - _meta_size;
    size_t block_size = get_occupied_block_size(block);
    
    if (get_occupied_block_allocator(block) != this) {
        std::string error = " this block is not from this allocator";
        error_with_guard(get_typename() + error);
        throw std::logic_error(error);
    }

    void* cur_avail = get_first_available_block();
    void* prev_avail = nullptr;
      
    while (cur_avail != nullptr) { 
        available_memory += get_available_block_size(cur_avail);
        void* cur_occup;
        if ((prev_avail == nullptr && cur_avail != get_first_block())
        || cur_avail == get_first_block()) {
            cur_occup = get_first_block();
        } else {
            cur_occup = reinterpret_cast<unsigned char *>(prev_avail) + get_available_block_size(prev_avail) + sizeof(void*) + sizeof(size_t);
        }
        while (cur_occup != cur_avail) {
            size_t occupied_size = get_occupied_block_size(cur_occup);

            if (cur_occup == block) {
                break;
            }
            cur_occup = reinterpret_cast<unsigned char *>(cur_occup) + sizeof(size_t) + sizeof(allocator*) + occupied_size;
        }
        if (cur_occup == block) {
            break;
        }
        prev_avail = cur_avail;
        cur_avail = get_available_block_next_block_address(cur_avail);
    }
    // if found block at the beginning
    if (block == get_first_block()) { 
        //if right block is available
        if (reinterpret_cast<unsigned char *>(block) + block_size + _meta_size == cur_avail) { 
            merge_blocks(1, block,  cur_avail);
        } else { // if right block is occupied
            size_t * old_size_ptr = reinterpret_cast<size_t*>(block);
            old_size_ptr = nullptr;
            void ** new_adress_ptr = reinterpret_cast<void**>(block);
            *new_adress_ptr = cur_avail;
            *reinterpret_cast<size_t*>(new_adress_ptr + 1) = block_size;
        }
        set_first_available_block(block);

        // getting blocks info
        std::vector<allocator_test_utils::block_info> blocks_info = get_blocks_info();
        print_blocks_info(blocks_info);

        debug_with_guard(get_typename() + " [END] " + func);
        return;
    }
    // if right is avail
    if (cur_avail == reinterpret_cast<unsigned char *>(block) +
    sizeof(size_t) + sizeof(allocator*) + block_size && cur_avail != nullptr) { 
        // merging
        merge_blocks(1, block, cur_avail);

        if (prev_avail != nullptr) {
            *reinterpret_cast<void**>(prev_avail) = block;

            if (reinterpret_cast<unsigned char *>(prev_avail) + sizeof(size_t) + 
            sizeof(void*) + get_available_block_size(prev_avail) == block) {
                //merge prev and cur
                merge_blocks(0, prev_avail, block);
            }
        } else {
            set_first_available_block(block);
        }
    } else if (cur_avail == reinterpret_cast<unsigned char *>(block) + sizeof(size_t) + 
    sizeof(allocator*) + block_size && cur_avail == nullptr) {
        size_t * old_size = reinterpret_cast<size_t*>(block);
        old_size = nullptr;
        void ** new_adress = reinterpret_cast<void**>(block);
        *new_adress = nullptr;
        *reinterpret_cast<size_t*>(new_adress + 1) = block_size;

        *reinterpret_cast<void**>(prev_avail) = block;

        // if left is free
        if (reinterpret_cast<unsigned char *>(prev_avail) + sizeof(size_t) + 
        sizeof(void*) + get_available_block_size(prev_avail) == block) { 
            merge_blocks(0, prev_avail, block);
        }
    } else { // if right is not avail
        size_t * old_size = reinterpret_cast<size_t*>(block);
        old_size = nullptr;
        void ** new_adress = reinterpret_cast<void**>(block);

        *new_adress = cur_avail;
        *reinterpret_cast<size_t*>(new_adress + 1) = block_size;


        if (prev_avail != nullptr) {
            *reinterpret_cast<void**>(prev_avail) = block;
            if (reinterpret_cast<unsigned char *>(prev_avail) + sizeof(size_t) + 
            sizeof(void*) + get_available_block_size(prev_avail) == block) { // if left is free
                merge_blocks(0, prev_avail, block);
            }
        } else {
            set_first_available_block(block);
        }
    }
    
    // gettign blocks info
    std::vector<allocator_test_utils::block_info> blocks_info = get_blocks_info();
    print_blocks_info(blocks_info);

    information_with_guard(get_typename() + " Available memory: " + std::to_string(available_memory));

    debug_with_guard(get_typename() + " [END] " + func);
}

void allocator_sorted_list::print_blocks_info(std::vector<allocator_test_utils::block_info> blocks_info) const noexcept {
    std::string str_block_info = "block status:\n";
    // printing info for each block
    for (auto block : blocks_info) {
        str_block_info += "\t<";
        if (block.is_block_occupied) str_block_info += "occup> ";
        else str_block_info += "avail> ";
        str_block_info += "<" + std::to_string(block.block_size) + ">\n";
    }
    debug_with_guard(str_block_info);
}

inline void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode mode) {
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator*) + sizeof(logger*) + sizeof(size_t)) = mode;
}

inline allocator_with_fit_mode::fit_mode allocator_sorted_list::get_fit_mode() {
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator*) + sizeof(logger*) + sizeof(size_t));
}

inline allocator *allocator_sorted_list::get_allocator() const {
    return *reinterpret_cast<allocator**>(_trusted_memory);
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept {
    void* cur_avail = get_first_available_block();
    void* prev_avail = nullptr;
    void* cur_occup = nullptr;
    void* prev_occup = nullptr;

    std::vector<allocator_test_utils::block_info> blocks_info;

    while (cur_avail != nullptr) {
        if (prev_avail == nullptr) {
            cur_occup = get_first_block();
        } else {
            cur_occup = reinterpret_cast<unsigned char *>(prev_avail) + sizeof(void*) + sizeof(size_t) + get_available_block_size(prev_avail);
        }

        while (cur_occup != cur_avail) {
            size_t occupied_size = get_occupied_block_size(cur_occup);
            allocator_test_utils::block_info occupied_block;
            occupied_block.block_size = occupied_size;
            occupied_block.is_block_occupied = true;
            blocks_info.push_back(occupied_block);

            prev_occup = cur_occup;
            cur_occup = reinterpret_cast<unsigned char *>(cur_occup) + sizeof(size_t) + sizeof(allocator*) + occupied_size;
       }
        allocator_test_utils::block_info available_block;
        available_block.block_size = get_available_block_size(cur_avail);
        available_block.is_block_occupied = false;
        blocks_info.push_back(available_block);

        prev_avail = cur_avail;
        cur_avail = get_available_block_next_block_address(cur_avail);
    }
    return blocks_info;
}

inline logger *allocator_sorted_list::get_logger() const {
    return *reinterpret_cast<logger**>(reinterpret_cast<allocator **>(_trusted_memory) + 1);
}

inline std::string allocator_sorted_list::get_typename() const noexcept {
    return "[allocator_sorted_list]";
}

void *allocator_sorted_list::get_first_available_block() const noexcept {
    return *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator *) + sizeof(logger *) + sizeof(size_t) + sizeof(allocator_with_fit_mode::fit_mode));
}

allocator::block_size_t allocator_sorted_list::get_available_block_size(void *block_address) const noexcept {
    return *reinterpret_cast<allocator::block_size_t *>(reinterpret_cast<void **>(block_address) + 1);
}

void *allocator_sorted_list::get_available_block_next_block_address(void *block_address) const noexcept {
    return *reinterpret_cast<void **>(block_address);
}

allocator::block_size_t allocator_sorted_list::get_occupied_block_size(void *block_address) const noexcept {
    return *reinterpret_cast<allocator::block_size_t *>(block_address);
}

void allocator_sorted_list::set_first_available_block(void * first_available_block) const noexcept {
    void ** first_block = reinterpret_cast<void**>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator*) + sizeof(logger*) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t));
    *first_block = first_available_block;
}

void * allocator_sorted_list::get_first_block() const noexcept {
    return reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator *) + sizeof(logger *) + sizeof(size_t) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(void*) + sizeof(std::mutex);
}

void allocator_sorted_list::clear_available_block(void * block) const noexcept {
    void ** adress = reinterpret_cast<void**>(block);
    size_t * size = reinterpret_cast<size_t*>(adress + 1);
    adress = nullptr;
    size = nullptr;
}

void allocator_sorted_list::merge_blocks(int status_free, void* first, void* second) noexcept {
    void* next_id = get_available_block_next_block_address(second);
    size_t next_size = get_available_block_size(second);

    size_t first_size;
    if (status_free == 0) { 
        first_size = get_available_block_size(first);
    } else {
        first_size = get_occupied_block_size(first);
        size_t * old_size = reinterpret_cast<size_t*>(first);
        old_size = nullptr;
        allocator ** alc = reinterpret_cast<allocator**>(old_size + 1);
        alc = nullptr;
    }
    void ** new_adress_ptr = reinterpret_cast<void**>(first);
    *new_adress_ptr = next_id;
    *reinterpret_cast<size_t*>(new_adress_ptr + 1) = first_size + next_size+ sizeof(void*) + sizeof(size_t);
    clear_available_block(second);
}

allocator * allocator_sorted_list::get_occupied_block_allocator(void * block) const noexcept {
    return *reinterpret_cast<allocator**>(reinterpret_cast<unsigned char *>(block) + sizeof(size_t));
}

std::mutex &allocator_sorted_list::get_mutex() const noexcept {
    return *reinterpret_cast<std::mutex *>(reinterpret_cast<unsigned char*>(_trusted_memory) + sizeof(allocator*) + sizeof(logger*) + sizeof(size_t) + sizeof(void*) + sizeof(allocator_with_fit_mode::fit_mode));
}