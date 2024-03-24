#include <not_implemented.h>

#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger){

}

allocator_global_heap::~allocator_global_heap() {

}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept {

}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept {

}

[[nodiscard]] void *allocator_global_heap::allocate(size_t value_size, size_t values_count) {

}

void allocator_global_heap::deallocate(void *at) {

}

inline logger *allocator_global_heap::get_logger() const {

}

inline std::string allocator_global_heap::get_typename() const noexcept {

}