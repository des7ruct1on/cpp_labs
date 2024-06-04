#include "../include/fraction.h"

big_integer fraction::greatest_common_divisor(big_integer const &one, big_integer const &another) noexcept
{
    // if zero
    if (another == big_integer("0")) {
        return one;
    }
    return greatest_common_divisor(another, one % another);
}

fraction &fraction::normalize_fraction() noexcept
{
    big_integer max = big_integer::max(big_integer::abs(_numerator), big_integer::abs(_denominator));
    big_integer min = big_integer::min(big_integer::abs(_numerator), big_integer::abs(_denominator));

    big_integer sign = (_numerator * _denominator >= big_integer("0")) ? big_integer("1") : big_integer("-1");
    
    big_integer gcd = greatest_common_divisor(max, min);
    //normalizing by euclidean
    _numerator = sign * (big_integer::abs(_numerator) / gcd);
    _denominator = big_integer::abs(_denominator) / gcd;
    return *this;
}

int fraction::factorial(int number) noexcept
{
    if (number == 0 || number == 1) {
        return 1;
    }
    return number * factorial(number - 1);
}

//binary pow from 3sem
int fraction::binary_power(int base, int power) noexcept
{
    if (!power)  {
        return base;
    }
    
    long long res = binary_power(base, power / 2);
    return base & 1 ? res * res * base : res * res;
}

fraction fraction::abs(fraction number) noexcept
{
    return number >= fraction(big_integer("0"), big_integer("1")) ? number : -number;
}

fraction::fraction(big_integer &&numerator, big_integer &&denominator):
    _numerator(std::forward<big_integer>(numerator)),
    _denominator(std::forward<big_integer>(denominator))
{
    if (_denominator == big_integer("0")) {
        throw std::invalid_argument("this fractiom has zero denominator!!!");
    }
    //normalazing
    normalize_fraction();
}

fraction::fraction(big_integer const &numerator, big_integer const &denominator):
    _numerator(numerator), _denominator(denominator)
{
    if (_denominator == big_integer("0")) {
        throw std::invalid_argument("this fractiom has zero denominator!!!");
    }
    //normalazing   
    normalize_fraction();
}

fraction &fraction::operator+=(fraction const &other)
{
    _numerator = _numerator * other._denominator + _denominator * other._numerator;
    _denominator *= other._denominator;
    return normalize_fraction();
}

fraction fraction::operator+(fraction const &other) const
{
    return fraction(*this) += other;
}

fraction &fraction::operator-=(fraction const &other)
{
    return *this = *this + fraction(-other._numerator, other._denominator);
}

fraction fraction::operator-(fraction const &other) const
{
    return fraction(*this) -= other;
}

fraction &fraction::operator-()
{
    _numerator = -_numerator;
    return normalize_fraction();
}

fraction &fraction::operator*=(fraction const &other)
{
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    return normalize_fraction();
}

fraction fraction::operator*(fraction const &other) const
{
    return fraction(*this) *= other;
}

fraction &fraction::operator/=(fraction const &other)
{
    return *this = other._numerator != big_integer("0") 
        ? *this * fraction(other._denominator, other._numerator)
        : fraction(big_integer("0"), big_integer("1"));
}

fraction fraction::operator/(fraction const &other) const
{
    return fraction(*this) /= other;
}

bool fraction::operator==(fraction const &other) const
{
    return _numerator == other._numerator && _denominator == other._denominator;
}

bool fraction::operator!=(fraction const &other) const
{
    return !(*this == other);
}

bool fraction::operator>=(fraction const &other) const
{
    return *this == other || *this > other;
}

bool fraction::operator>(fraction const &other) const
{
    return (_numerator * other._denominator) > (other._numerator * _denominator);
}

bool fraction::operator<=(fraction const &other) const
{
    return !(*this > other);
}

bool fraction::operator<(fraction const &other) const
{
    return !(*this >= other);
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    stream << obj._numerator;
    stream << "/" << obj._denominator;
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    std::string value;
    stream >> value;

    fraction input_fraction = obj;

    bool has_denominator = true;
    size_t pos = value.find('/');
    if (pos == std::string::npos) {
        has_denominator = false;
    }

    auto numerator = value.substr(0, pos);
    auto denominator = has_denominator ? value.substr(pos + 1) : "1";

    input_fraction._numerator = std::move(big_integer(numerator));
    input_fraction._denominator = std::move(big_integer(denominator));
    
    obj = std::move(input_fraction);

    return stream;
}

fraction fraction::sin(fraction const &epsilon) const
{
    if (fraction::abs(*this) > fraction(big_integer("1"), big_integer("1"))) {
        std::cout << fraction::abs(*this) << std::endl;
        throw std::invalid_argument("fraction is not in [-1, 1]");
    }

    int iteration = 0;
    int sign = 1;
    
    fraction prev(big_integer("0"), big_integer("1"));
    fraction sum(big_integer("0"), big_integer("1"));

    do {
        prev = sum;
        sum += fraction(big_integer(std::vector<int> { sign }), big_integer(std::vector<int> { factorial(2 * iteration + 1) })) * pow(2 * iteration + 1);
        
        sign *= -1;
        ++iteration;
    }
    while (fraction::abs(sum - prev) > epsilon);

    return sum.normalize_fraction();
}

fraction fraction::cos(fraction const &epsilon) const
{
    if (fraction::abs(*this) > fraction(big_integer("1"), big_integer("1"))) {
        throw std::invalid_argument("fraction is not in [-1, 1]");
    }
    
    int iteration = 0;
    int sign = 1;
    
    fraction prev(big_integer("0"), big_integer("1"));
    fraction sum(big_integer("0"), big_integer("1"));

    do {
        prev = sum;
        sum += fraction(big_integer(std::vector<int> { sign }), big_integer(std::vector<int> { factorial(2 * iteration) })) * pow(2 * iteration);
        
        sign *= -1;
        ++iteration;
    }
    while (fraction::abs(sum - prev) > epsilon);

    return sum.normalize_fraction();
}

fraction fraction::tg(fraction const &epsilon) const
{
    return sin(epsilon) / cos(epsilon);
}

fraction fraction::ctg(fraction const &epsilon) const
{
    return cos(epsilon) / sin(epsilon);
}

fraction fraction::sec(fraction const &epsilon) const
{
    fraction cos_ = cos(epsilon);
    return fraction(cos_._denominator, cos_._numerator);
}

fraction fraction::cosec(fraction const &epsilon) const
{
    fraction sin_ = sin(epsilon);
    return fraction(sin_._denominator, sin_._numerator);
}
// arcsin(x) = x + (1/2)x^3/3 + (1*3)/(2*4)x^5/5 + (1*3*5)/(2*4*6)x^7/7 + ...
fraction fraction::arcsin(fraction const &epsilon) const
{
    if (fraction::abs(*this) > fraction(big_integer("1"), big_integer("1"))) {
        throw std::invalid_argument("fraction is not in [-1, 1]");
    }
    
    int iteration = 1;

    fraction prev(big_integer("1"), big_integer("1"));
    fraction result = *this;

    while (abs(prev) > epsilon) {
        int precalc = 1;
		for (size_t i = 1; i < 2 * i - 1; i += 2) {
			precalc *= i;
		}
        
        prev = fraction(big_integer("2"), big_integer("1")).pow(iteration);
        prev *= fraction(big_integer::factorial(big_integer(std::vector<int> { iteration })), big_integer("1"));
        prev *= fraction(big_integer(std::vector<int> { 2 * iteration + 1} ), big_integer("1"));
        prev = fraction(big_integer("1"), big_integer("1")) / prev;
        prev *= fraction(big_integer(std::vector<int> { precalc }), big_integer("1"));
        prev *= pow(2 * iteration + 1);

        result += prev;
        ++iteration;
    }

    return result.normalize_fraction();
}
// arccos(x) = 2 * arcsin(sqrt(1 - x^2))
fraction fraction::arccos(fraction const &epsilon) const
{
    return fraction(big_integer("2"), big_integer("1")) * (fraction(big_integer("1"), big_integer("1")) - *this * fraction(big_integer("1"), big_integer("2")).root(2, epsilon)).arcsin(epsilon);
}

fraction fraction::arctg(fraction const &epsilon) const
{
    return (*this / (pow(2) + fraction(big_integer("1"), big_integer("1"))).root(2, epsilon)).arcsin(epsilon);
}

fraction fraction::arcctg(fraction const &epsilon) const
{
    return (*this * (fraction(big_integer("1"), big_integer("1")) / ((*this) * (*this) + fraction(big_integer("1"), big_integer("1"))).root(2, epsilon))).arccos(epsilon);
}

fraction fraction::arcsec(fraction const &epsilon) const
{
    return (fraction(big_integer("1"), big_integer("1")) / *this).arccos(epsilon);
}

fraction fraction::arccosec(fraction const &epsilon) const
{
    return (fraction(big_integer("1"), big_integer("1")) / *this).arcsin(epsilon);
}

fraction fraction::pow(size_t degree) const
{
    if (!degree) {
        return fraction(big_integer("1"), big_integer("1"));
    }

    fraction res = this->pow(degree / 2);
    return degree & 1 ? res * res * *this : res * res;
}

//√x = x - x^3/2 + x^5*3/(2*4) - x^7*15/(2*4*6) + ...
fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    fraction copy = *this;
    bool swapped = false;
    if (copy._numerator > copy._denominator) {
        std::swap(copy._numerator, copy._denominator);
        swapped = true;
    }

    fraction alpha = fraction(big_integer("1"), big_integer(std::to_string(degree)));
    copy -= fraction(big_integer("1"), big_integer("1"));

    fraction sum(big_integer("1"), big_integer("1"));
    fraction prev = fraction(big_integer("2"), big_integer("1")) * epsilon;
    int iteration = 1;

    fraction precompute = alpha;
    while (abs(prev) > epsilon) {
        prev = precompute;
        prev *= copy.pow(iteration);
        prev *= fraction(big_integer("1"), big_integer::factorial(big_integer(std::vector<int> { iteration })));
        sum += prev;

        precompute *= (alpha - fraction(big_integer(std::to_string(iteration)), big_integer("1")));
		++iteration;
    }
    
    if (swapped) {
        std::swap(sum._denominator, sum._numerator);
    }

    return sum.normalize_fraction();
}

//log2(x) = ln(x) / ln(2)
fraction fraction::log2(fraction const &epsilon) const
{
    return ln(epsilon) / (fraction(big_integer("2"), big_integer("1"))).ln(epsilon);
}

fraction fraction::ln(fraction const &epsilon) const
{
    fraction copy = *this;

    bool swapped = false;
    if (copy._numerator > copy._denominator) {
        std::swap(copy._numerator, copy._denominator);
        swapped = true;
    }

    copy -= fraction(big_integer("1"), big_integer("1"));

	fraction sum = fraction(big_integer("0"), big_integer("1"));
	fraction prev = fraction(big_integer("2"), big_integer("1")) * epsilon;
	
    size_t iteration = 1;
	int sign = 1;

    while (abs(prev) > epsilon) {
		prev = copy.pow(iteration);
		prev *= fraction(big_integer(std::to_string(sign)), big_integer(std::vector<int> { static_cast<int>(iteration) }));

		sum += prev;
		++iteration;
		sign *= -1;
	}

	if (swapped) {
		sum *= fraction(big_integer("-1"), big_integer("1"));
	}

    return sum.normalize_fraction();
}

fraction fraction::lg(fraction const &epsilon) const
{
    return ln(epsilon) / (fraction(big_integer("10"), big_integer("1"))).ln(epsilon);
}