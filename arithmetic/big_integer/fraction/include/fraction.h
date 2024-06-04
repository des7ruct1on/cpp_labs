#ifndef MP_OS_FRACTION_H
#define MP_OS_FRACTION_H

#include <big_integer.h>

class fraction final
{

private:

    big_integer _numerator;
    big_integer _denominator;

private:

    static big_integer greatest_common_divisor(big_integer const &one, big_integer const &another) noexcept;

    fraction &normalize_fraction() noexcept;

    static int factorial(int number) noexcept;

    static fraction abs(fraction number) noexcept;

    static int binary_power(int base, int power) noexcept;

public:

    fraction(big_integer &&numerator, big_integer &&denominator);

    fraction(big_integer const &numerator, big_integer const &denominator);

public:

    ~fraction() noexcept = default;

    fraction(fraction const &other) = default;

    fraction &operator=(fraction const &other) = default;

    fraction(fraction &&other) noexcept = default;

    fraction &operator=(fraction &&other) noexcept = default;

public:

    fraction &operator+=(fraction const &other);

    fraction operator+(fraction const &other) const;

    fraction &operator-=(fraction const &other);

    fraction operator-(fraction const &other) const;
    
    fraction &operator-();

    fraction &operator*=(fraction const &other);

    fraction operator*(fraction const &other) const;

    fraction &operator/=(fraction const &other);

    fraction operator/(fraction const &other) const;

public:

    bool operator==(fraction const &other) const;

    bool operator!=(fraction const &other) const;

public:

    bool operator>=(fraction const &other) const;

    bool operator>(fraction const &other) const;

    bool operator<=(fraction const &other) const;

    bool operator<(fraction const &other) const;

public:

    friend std::ostream &operator<<(std::ostream &stream, fraction const &obj);

    friend std::istream &operator>>(std::istream &stream, fraction &obj);

public:

    fraction sin(fraction const &epsilon) const;

    fraction cos(fraction const &epsilon) const;

    fraction tg(fraction const &epsilon) const;

    fraction ctg(fraction const &epsilon) const;

    fraction sec(fraction const &epsilon) const;

    fraction cosec(fraction const &epsilon) const;

    fraction arcsin(fraction const &epsilon) const;

    fraction arccos(fraction const &epsilon) const;

    fraction arctg(fraction const &epsilon) const;

    fraction arcctg(fraction const &epsilon) const;

    fraction arcsec(fraction const &epsilon) const;

    fraction arccosec(fraction const &epsilon) const;

public:

    fraction pow(size_t degree) const;

public:

    fraction root(size_t degree, fraction const &epsilon) const;

public:

    fraction log2(fraction const &epsilon) const;

    fraction ln(fraction const &epsilon) const;

    fraction lg(fraction const &epsilon) const;

};

#endif //MP_OS_FRACTION_H