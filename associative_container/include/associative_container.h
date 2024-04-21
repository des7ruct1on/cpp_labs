#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_ASSOCIATIVE_CONTAINER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_ASSOCIATIVE_CONTAINER_H

#include <iostream>
#include <vector>
#include <operation_not_supported.h>

template<typename tkey, typename tvalue>
class associative_container {

public:
    
    struct key_value_pair final {
        tkey key;
        tvalue value;
        key_value_pair(tkey const &key, tvalue const &value): key(key), value(value) {

        }

        key_value_pair(tkey const &key, tvalue &&value): key(key), value(std::move(value)) {

        }

        key_value_pair(key_value_pair const &other): key(other.key), value(other.value) {
            int x = 10;
        }

        key_value_pair(key_value_pair &&other) noexcept: key(other.key), value(std::move(other.value)) {

        }

        key_value_pair &operator=(key_value_pair const &other) {
            if (this != &other) {
                key = other.key;
                value = other.value;
            }

            return *this;
        }



        key_value_pair &operator=(key_value_pair &&other) noexcept {
            if (this != &other)
            {
                key = other.key;
                value = std::move(other.value);
            }

            return *this;
        }

        ~key_value_pair() {
            int x = 10;
        }
        
    };
    
    struct key_value_ptr_pair {      
        tkey key;
        tvalue *value_ptr;
        key_value_ptr_pair(tkey const &key, tvalue *value_ptr): key(key), value_ptr(value_ptr) {

        }
        
    };

public:
    
    virtual ~associative_container() noexcept = default;

public:
    
    virtual void insert(tkey const &key, tvalue const &value) = 0;
    
    virtual void insert(tkey const &key, tvalue &&value) = 0;
    
    virtual tvalue const &obtain(tkey const &key) = 0;
    
    virtual tvalue dispose(tkey const &key) = 0;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_TEMPLATE_REPO_ASSOCIATIVE_CONTAINER_H
