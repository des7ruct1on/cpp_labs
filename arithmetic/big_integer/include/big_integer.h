#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H

#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include <algorithm>

#include <allocator.h>
#include <allocator_guardant.h>
#include <not_implemented.h>
#include <cstdint>
#include <limits>

class big_integer final: allocator_guardant
{

public:
    
    enum class multiplication_rule
    {
        trivial,
        Karatsuba,
        SchonhageStrassen
    };

private:
    
    class multiplication
    {
    
    public:
        
        virtual ~multiplication() noexcept = default;
    
    public:
        
        virtual big_integer &multiply(big_integer &first_multiplier, big_integer const &second_multiplier) const = 0;
        
    };
    
    class trivial_multiplication final: public multiplication
    {
    
    public:
        
        big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const override;
        
    };
    
    class Karatsuba_multiplication final:
        public multiplication
    {

    public:
        
        big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const override;
        
    };
    
    class Schonhage_Strassen_multiplication final:
        public multiplication
    {

    public:
        
        big_integer &multiply(big_integer &first_multiplier, big_integer const &second_multiplier) const override;
        
    };

public:
    
    enum class division_rule
    {
        trivial,
        Newton,
        BurnikelZiegler
    };

private:
    
    class division
    {
    
    public:
        
        virtual ~division() noexcept = default;
    
    public:
        
        virtual big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const = 0;
        
        virtual big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const = 0;
        
    };
    
    class trivial_division final: public division
    {

    
    public:
        
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
    };
    
    class Newton_division final:
        public division
    {
    
    public:
        
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
    };
    
    class Burnikel_Ziegler_division final: public division
    {
    
    public:
        
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
    };

private:

    int _oldest_digit;
    unsigned int *_other_digits = nullptr;

    allocator *_allocator = nullptr;

private:

    void copy_from(big_integer const &other);

    void initialize_from(int const *digits, size_t digits_count);

    void initialize_from(std::vector<int> const &digits, size_t digits_count);

    void initialize_from(std::string const &value_as_string, size_t base = 10);

public:

    big_integer(int const *digits, size_t digits_count, allocator *allocator = nullptr);

    explicit big_integer(std::vector<int> const &digits, allocator *allocator = nullptr);

    explicit big_integer(std::string const &value_as_string, size_t base = 10, allocator *allocator = nullptr);

    explicit big_integer(uint number);

    explicit big_integer(std::vector<unsigned int> const &digits);

public:

    ~big_integer() noexcept;
    
    big_integer(big_integer const &other);

    big_integer &operator=(big_integer const &other);
    
    big_integer(big_integer &&other) noexcept;
    
    big_integer &operator=(big_integer &&other) noexcept;

public:

    bool operator==(big_integer const &other) const;

    bool operator!=(big_integer const &other) const;

public:

    bool operator<(big_integer const &other) const;

    bool operator>(big_integer const &other) const;

    bool operator<=(big_integer const &other) const;

    bool operator>=(big_integer const &other) const;

public:

    big_integer operator-() const;
    
    big_integer &operator+=(big_integer const &other);

    big_integer operator+(big_integer const &other) const;

    big_integer operator+(std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator-=(big_integer const &other);

    big_integer operator-(big_integer const &other) const;

    big_integer operator-(std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator*=(big_integer const &other);

    big_integer operator*(big_integer const &other) const;

    big_integer operator*(std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator/=(big_integer const &other);

    big_integer operator/(big_integer const &other) const;

    big_integer operator/(std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator%=(big_integer const &other);

    big_integer operator%(big_integer const &other) const;

    big_integer operator%(std::pair<big_integer, allocator *> const &other) const;

public:

    big_integer operator~() const;

    big_integer &operator&=(big_integer const &other);

    big_integer operator&(big_integer const &other) const;

    big_integer operator&(std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator|=(big_integer const &other);

    big_integer operator|(big_integer const &other) const;

    big_integer operator|(std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator^=(big_integer const &other);

    big_integer operator^(big_integer const &other) const;

    big_integer operator^(std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator<<=(size_t shift);

    big_integer operator<<(size_t shift) const;

    big_integer operator<<(std::pair<size_t, allocator *> const &shift) const;

    big_integer &operator>>=(size_t shift);

    big_integer operator>>(size_t shift) const;

    big_integer operator>>(std::pair<size_t, allocator *> const &shift) const;

public:

    static big_integer &multiply(
        big_integer &first_multiplier,
        big_integer const &second_multiplier,
        allocator *allocator = nullptr,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer multiply(
        big_integer const &first_multiplier,
        big_integer const &second_multiplier,
        allocator *allocator = nullptr,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &divide(
        big_integer &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer divide(
        big_integer const &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &modulo(
        big_integer &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer modulo(
        big_integer const &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

public:
    
    friend std::ostream &operator<<(std::ostream &stream, big_integer const &value);
    
    friend std::istream &operator>>(std::istream &stream, big_integer &value);

public:

    size_t default_base = 1 << (8 * sizeof(int) - 1);

private:

    [[nodiscard]] allocator *get_allocator() const noexcept override;

public:

    inline int sign() const noexcept;

    inline bool is_zero() const noexcept;

    inline unsigned int get_digit(int index) const noexcept;

    inline int get_size() const noexcept;

    big_integer &change_sign();

    big_integer abs(big_integer const &number) noexcept;
    
    big_integer max(big_integer const &one, big_integer const &another) noexcept;
    
    big_integer min(big_integer const &one, big_integer const &another) noexcept;
    
    big_integer factorial(big_integer const &number);
    
    inline bool is_one() const noexcept;

private:

    std::vector<int> convert_string_to_vector(std::string value_as_string, size_t index);

    void clear();

    std::vector<int> convert_to_base(std::string const &value, size_t base);

public:

    std::string big_integer_to_string(big_integer const & value) const;

    std::string string_to_decimal(const std::string& number, int base);

private:

    int big_int_cmp(big_integer const & first, big_integer const & second) const;
    inline unsigned int get_digit_big_endian(int position) const noexcept;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H