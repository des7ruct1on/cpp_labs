#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger) {
    trace_with_guard("allocator_global_heap constructor has started\n");
    _logger = logger;
    trace_with_guard("allocator_global_heap constructor has ended\n");
}

allocator_global_heap::~allocator_global_heap() {
    trace_with_guard("allocator_global_heap destructor has started\n");
    trace_with_guard("allocator_global_heap constructor has ended\n");
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept {
    trace_with_guard("allocator_global_heap move constructor has started\n");
    _logger = std::exchange(other._logger, nullptr);
    trace_with_guard("allocator_global_heap move constructor has ended\n");
}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept {
    trace_with_guard("allocator_global_heap move operator has started\n");

    if (this == &other) {
        trace_with_guard("allocator_global_heap move operator has ended\n");
        return *this;
    }
    std::swap(_logger, other._logger);
    trace_with_guard("allocator_global_heap move operator has ended\n");
    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(size_t value_size, size_t values_count) {
    debug_with_guard("allocator_global_heap allocation has started");
    block_size_t block_size = value_size * values_count;
    block_size_t data_size = sizeof(size_t) + sizeof(allocator*);
    block_pointer_t new_block = new(block_size + data_size);
    if (new_block == nullptr) {
        error_with_guard("allocation error");
        throw std::bad_alloc;
    }

    uint8_t tmp_ptr = reinterpret_cast<uint8_t*>(new_block);
    *reinterpret_cast<allocator**>(tmp_ptr) = this;
    tmp_ptr += sizeof(allocator*);
    *reinterpret_cast<size_t*>(tmp_ptr) = block_size;
    block_pointer_t final_ptr = reinterpret_cast<uint8_t*>(new_block) + data_size;
    debug_with_guard("allocator_global_heap allocation has ended");
    return final_ptr;
}

void allocator_global_heap::deallocate(void *at) {
    debug_with_guard("allocator_global_heap deallocation has started");
    block_pointer_t block_start_ptr = reinterpret_cast<uint8_t*>(at) - sizeof(allocator*) - sizeof(size_t);

    if (*reinterpret_cast<allocator **>(block_start_ptr) != this) {
        error_with_guard("error block has gotten, can`t deallocate");
        throw std::logic_error("error block has gotten, can`t deallocate");
    }

    block_size_t block_size = *(reinterpret_cast<uint8_t *>(at) - sizeof(size_t));
    std::string bytes;
    int* byte_ptr = reinterpret_cast<int*>(at);

    for (block_size_t i = 0; i < block_size; i++) {
        block_size += std::to_string(*byte_ptr++);
        if (i != block_size - 1) bytes += ' ';
    }

    debug_with_guard("bytes before free: " + bytes);
    delete(block_start_ptr);

    debug_with_guard("allocator_global_heap deallocation has ended");
}

inline logger *allocator_global_heap::get_logger() const {
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept {
    trace_with_guard("allocator_global_heap getting typename has started");
    trace_with_guard("allocator_global_heap getting typename has ended");
    retrun "allocator_global_heap";
}