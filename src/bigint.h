#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace at {

// Arbitrary-precision integer (BigInt). Stored as sign + magnitude in base 2^32.
class BigInt {
public:
    BigInt();
    BigInt(int64_t value);
    BigInt(const std::string& decimal);

    // Arithmetic
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    BigInt operator-() const; // negation

    // Comparison
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    // Conversion
    std::string toString() const;
    int64_t toInt64() const; // truncates if too large
    double toDouble() const;

    // Predicates
    bool isZero() const;
    bool isNegative() const;
    bool isPositive() const;

    // Bitwise (for future use)
    BigInt operator&(const BigInt& other) const;
    BigInt operator|(const BigInt& other) const;
    BigInt operator^(const BigInt& other) const;

private:
    bool negative_ = false;
    std::vector<uint32_t> mag_; // magnitude in base 2^32, little-endian

    void normalize();
    static int compareMagnitude(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
    friend void divmod(const BigInt& a, const BigInt& b, BigInt& q, BigInt& r);
};

} // namespace at
