#include "bigint.h"

#include <algorithm>
#include <stdexcept>

namespace at {

BigInt::BigInt() : negative_(false) {
    mag_.push_back(0);
}

BigInt::BigInt(int64_t value) {
    if (value < 0) {
        negative_ = true;
        value = -value;
    } else {
        negative_ = false;
    }
    uint64_t v = (uint64_t)value;
    if (v == 0) {
        mag_.push_back(0);
    } else {
        while (v > 0) {
            mag_.push_back((uint32_t)(v & 0xFFFFFFFF));
            v >>= 32;
        }
    }
    normalize();
}

BigInt::BigInt(const std::string& decimal) {
    negative_ = false;
    mag_.push_back(0);

    size_t i = 0;
    if (i < decimal.size() && (decimal[i] == '-' || decimal[i] == '+')) {
        if (decimal[i] == '-') negative_ = true;
        ++i;
    }

    for (; i < decimal.size(); ++i) {
        char c = decimal[i];
        if (c < '0' || c > '9') throw std::invalid_argument("Invalid BigInt string");
        int digit = c - '0';
        uint64_t carry = digit;
        for (size_t j = 0; j < mag_.size(); ++j) {
            uint64_t val = (uint64_t)mag_[j] * 10 + carry;
            mag_[j] = (uint32_t)(val & 0xFFFFFFFF);
            carry = val >> 32;
        }
        while (carry > 0) {
            mag_.push_back((uint32_t)(carry & 0xFFFFFFFF));
            carry >>= 32;
        }
    }
    normalize();
}

void BigInt::normalize() {
    while (mag_.size() > 1 && mag_.back() == 0) mag_.pop_back();
    if (mag_.size() == 1 && mag_[0] == 0) negative_ = false;
}

int BigInt::compareMagnitude(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
    }
    return 0;
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (negative_ == other.negative_) {
        BigInt result;
        result.negative_ = negative_;
        size_t n = std::max(mag_.size(), other.mag_.size());
        uint64_t carry = 0;
        result.mag_.resize(n);
        for (size_t i = 0; i < n; ++i) {
            uint64_t a = i < mag_.size() ? mag_[i] : 0;
            uint64_t b = i < other.mag_.size() ? other.mag_[i] : 0;
            uint64_t sum = a + b + carry;
            result.mag_[i] = (uint32_t)(sum & 0xFFFFFFFF);
            carry = sum >> 32;
        }
        if (carry) result.mag_.push_back((uint32_t)carry);
        result.normalize();
        return result;
    }
    int cmp = compareMagnitude(mag_, other.mag_);
    if (cmp == 0) return BigInt(0);
    if (cmp > 0) {
        BigInt result;
        result.negative_ = negative_;
        uint64_t borrow = 0;
        result.mag_.resize(mag_.size());
        for (size_t i = 0; i < mag_.size(); ++i) {
            uint64_t a = mag_[i];
            uint64_t b = i < other.mag_.size() ? other.mag_[i] : 0;
            if (a < b + borrow) {
                result.mag_[i] = (uint32_t)((a + (1ULL << 32)) - b - borrow);
                borrow = 1;
            } else {
                result.mag_[i] = (uint32_t)(a - b - borrow);
                borrow = 0;
            }
        }
        result.normalize();
        return result;
    } else {
        BigInt result;
        result.negative_ = other.negative_;
        uint64_t borrow = 0;
        result.mag_.resize(other.mag_.size());
        for (size_t i = 0; i < other.mag_.size(); ++i) {
            uint64_t a = other.mag_[i];
            uint64_t b = i < mag_.size() ? mag_[i] : 0;
            if (a < b + borrow) {
                result.mag_[i] = (uint32_t)((a + (1ULL << 32)) - b - borrow);
                borrow = 1;
            } else {
                result.mag_[i] = (uint32_t)(a - b - borrow);
                borrow = 0;
            }
        }
        result.normalize();
        return result;
    }
}

BigInt BigInt::operator-(const BigInt& other) const {
    BigInt neg = other;
    neg.negative_ = !neg.negative_;
    if (other.isZero()) neg.negative_ = false;
    return *this + neg;
}

BigInt BigInt::operator*(const BigInt& other) const {
    if (isZero() || other.isZero()) return BigInt(0);

    BigInt result;
    result.negative_ = (negative_ != other.negative_);
    result.mag_.assign(mag_.size() + other.mag_.size(), 0);

    for (size_t i = 0; i < mag_.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.mag_.size(); ++j) {
            uint64_t prod = (uint64_t)mag_[i] * other.mag_[j] + result.mag_[i + j] + carry;
            result.mag_[i + j] = (uint32_t)(prod & 0xFFFFFFFF);
            carry = prod >> 32;
        }
        result.mag_[i + other.mag_.size()] += (uint32_t)carry;
    }
    result.normalize();
    return result;
}

// Schoolbook long division
void divmod(const BigInt& a, const BigInt& b, BigInt& q, BigInt& r) {
    // Work with magnitudes
    std::vector<uint32_t> u = a.mag_;
    std::vector<uint32_t> v = b.mag_;
    // Remove leading zeros
    while (u.size() > 1 && u.back() == 0) u.pop_back();
    while (v.size() > 1 && v.back() == 0) v.pop_back();

    int m = (int)u.size();
    int n = (int)v.size();

    if (m < n) {
        q = BigInt(0);
        r = a;
        r.negative_ = false;
        return;
    }

    // Normalize: ensure high bit of v is set
    int shift = 0;
    uint32_t highV = v.back();
    while ((highV & 0x80000000) == 0) {
        highV <<= 1;
        ++shift;
    }

    // Shift u and v
    // (simplified: just do digit-by-digit for reasonable sizes)
    // Use base-2^32 long division

    // If v has 1 digit, simple division
    if (n == 1) {
        uint64_t rem = 0;
        std::vector<uint32_t> qmag(m);
        for (int i = m - 1; i >= 0; --i) {
            uint64_t cur = (rem << 32) | u[i];
            qmag[i] = (uint32_t)(cur / v[0]);
            rem = cur % v[0];
        }
        q.mag_ = qmag;
        q.negative_ = false;
        r.mag_.clear();
        r.mag_.push_back((uint32_t)rem);
        r.negative_ = false;
        q.normalize();
        r.normalize();
        return;
    }

    // General case: use Knuth Algorithm D (simplified)
    // For simplicity, fall back to repeated subtraction for small quotients
    // but use a better method for large
    BigInt absA = a; absA.negative_ = false;
    BigInt absB = b; absB.negative_ = false;
    BigInt qq(0);
    BigInt rr = absA;
    // Only do this if the quotient is small enough
    // Estimate quotient size
    if (m - n < 2) {
        // Small quotient: repeated subtraction
        while (rr >= absB) {
            rr = rr - absB;
            qq = qq + BigInt(1);
        }
        q = qq;
        r = rr;
        q.normalize();
        r.normalize();
        return;
    }

    // For large quotients, use binary search on quotient
    // This is O(log(q) * M(n)) which is reasonable
    BigInt lo(0), hi = absA;
    // hi = absA / 2 + 1 as upper bound (rough)
    // Actually, quotient <= absA, so hi = absA
    // Binary search
    while (lo < hi) {
        BigInt mid = (lo + hi + BigInt(1)) / BigInt(2);
        BigInt prod = mid * absB;
        if (prod <= absA) {
            lo = mid;
        } else {
            hi = mid - BigInt(1);
        }
    }
    q = lo;
    r = absA - q * absB;
    q.normalize();
    r.normalize();
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other.isZero()) throw std::runtime_error("Division by zero");

    BigInt q, r;
    divmod(*this, other, q, r);
    q.negative_ = (negative_ != other.negative_);
    if (q.isZero()) q.negative_ = false;
    return q;
}

BigInt BigInt::operator%(const BigInt& other) const {
    if (other.isZero()) throw std::runtime_error("Division by zero");

    BigInt q, r;
    divmod(*this, other, q, r);
    r.negative_ = negative_;
    if (r.isZero()) r.negative_ = false;
    return r;
}

BigInt BigInt::operator-() const {
    BigInt result = *this;
    if (!isZero()) result.negative_ = !negative_;
    return result;
}

bool BigInt::operator==(const BigInt& other) const {
    if (isZero() && other.isZero()) return true;
    if (negative_ != other.negative_) return false;
    return mag_ == other.mag_;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    if (negative_ && !other.negative_) return true;
    if (!negative_ && other.negative_) return false;
    if (negative_) {
        return compareMagnitude(mag_, other.mag_) > 0;
    }
    return compareMagnitude(mag_, other.mag_) < 0;
}

bool BigInt::operator<=(const BigInt& other) const {
    return *this < other || *this == other;
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

std::string BigInt::toString() const {
    if (isZero()) return "0";
    BigInt absVal = *this; absVal.negative_ = false;
    std::string result;
    BigInt ten(10);
    BigInt val = absVal;
    while (!val.isZero()) {
        BigInt digit = val % ten;
        result.push_back('0' + (char)digit.toInt64());
        val = val / ten;
    }
    if (negative_) result.push_back('-');
    std::reverse(result.begin(), result.end());
    return result;
}

int64_t BigInt::toInt64() const {
    if (isZero()) return 0;
    uint64_t result = 0;
    for (int i = (int)mag_.size() - 1; i >= 0; --i) {
        result = (result << 32) | mag_[i];
    }
    if (negative_) return -(int64_t)result;
    return (int64_t)result;
}

double BigInt::toDouble() const {
    return (double)toInt64();
}

bool BigInt::isZero() const {
    return mag_.size() == 1 && mag_[0] == 0;
}

bool BigInt::isNegative() const {
    return negative_ && !isZero();
}

bool BigInt::isPositive() const {
    return !negative_ && !isZero();
}

BigInt BigInt::operator&(const BigInt& other) const {
    BigInt result;
    result.negative_ = negative_ && other.negative_;
    size_t n = std::min(mag_.size(), other.mag_.size());
    result.mag_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        result.mag_[i] = mag_[i] & other.mag_[i];
    }
    result.normalize();
    return result;
}

BigInt BigInt::operator|(const BigInt& other) const {
    BigInt result;
    result.negative_ = negative_ || other.negative_;
    size_t n = std::max(mag_.size(), other.mag_.size());
    result.mag_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        uint32_t a = i < mag_.size() ? mag_[i] : 0;
        uint32_t b = i < other.mag_.size() ? other.mag_[i] : 0;
        result.mag_[i] = a | b;
    }
    result.normalize();
    return result;
}

BigInt BigInt::operator^(const BigInt& other) const {
    BigInt result;
    result.negative_ = (negative_ != other.negative_);
    size_t n = std::max(mag_.size(), other.mag_.size());
    result.mag_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        uint32_t a = i < mag_.size() ? mag_[i] : 0;
        uint32_t b = i < other.mag_.size() ? other.mag_[i] : 0;
        result.mag_[i] = a ^ b;
    }
    result.normalize();
    if (result.isZero()) result.negative_ = false;
    return result;
}

} // namespace at
