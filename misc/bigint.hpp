#pragma once

#include <cstdint>
#include <gmp.h>
#include <istream>
#include <ostream>
#include <vector>

class BigInt {
  mpz_t value;

public:
  BigInt() { mpz_init(value); }
  BigInt(const BigInt &that) { mpz_init_set(value, that.value); }
  BigInt(BigInt &&that) {
    value[0] = that.value[0];
    that.value[0]._mp_d = nullptr;
  }
  BigInt &operator=(const BigInt &that) {
    if (this != &that) {
      mpz_set(value, that.value);
    }
    return *this;
  }
  BigInt &operator=(BigInt &&that) {
    if (this != &that) {
      value[0] = that.value[0];
      that.value[0]._mp_d = nullptr;
    }
    return *this;
  }
  BigInt(const std::string &str) { mpz_init_set_str(value, str.c_str(), 10); }
  BigInt(int n) { mpz_init_set_si(value, n); }
  ~BigInt() {
    if (value[0]._mp_d)
      mpz_clear(value);
  }
  // calculation
  BigInt operator+(const BigInt &that) const {
    BigInt result;
    mpz_add(result.value, value, that.value);
    return result;
  }
  BigInt operator-(const BigInt &that) const {
    BigInt result;
    mpz_sub(result.value, value, that.value);
    return result;
  }
  BigInt operator*(const BigInt &that) const {
    BigInt result;
    mpz_mul(result.value, value, that.value);
    return result;
  }
  BigInt operator/(const BigInt &that) const {
    BigInt result;
    mpz_div(result.value, value, that.value);
    return result;
  }
  BigInt operator%(const BigInt &that) const {
    BigInt result;
    mpz_mod(result.value, value, that.value);
    return result;
  }
  BigInt &operator+=(const BigInt &that) {
    mpz_add(value, value, that.value);
    return *this;
  }
  BigInt &operator-=(const BigInt &that) {
    mpz_sub(value, value, that.value);
    return *this;
  }
  BigInt &operator*=(const BigInt &that) {
    mpz_mul(value, value, that.value);
    return *this;
  }
  BigInt &operator/=(const BigInt &that) {
    mpz_div(value, value, that.value);
    return *this;
  }
  BigInt &operator%=(const BigInt &that) {
    mpz_mod(value, value, that.value);
    return *this;
  }
  // comparing
  bool operator==(const BigInt &that) const {
    return mpz_cmp(value, that.value) == 0;
  }
  bool operator!=(const BigInt &that) const {
    return mpz_cmp(value, that.value) != 0;
  }
  bool operator<(const BigInt &that) const {
    return mpz_cmp(value, that.value) < 0;
  }
  bool operator<=(const BigInt &that) const {
    return mpz_cmp(value, that.value) <= 0;
  }
  bool operator>(const BigInt &that) const {
    return mpz_cmp(value, that.value) > 0;
  }
  bool operator>=(const BigInt &that) const {
    return mpz_cmp(value, that.value) >= 0;
  }
  // unary
  BigInt operator-() const {
    BigInt result;
    mpz_neg(result.value, value);
    return result;
  }
  // io
  friend std::ostream &operator<<(std::ostream &os, const BigInt &that) {
    os << mpz_get_str(nullptr, 10, that.value);
    return os;
  }
  friend std::istream &operator>>(std::istream &is, BigInt &that) {
    std::string str;
    is >> str;
    mpz_set_str(that.value, str.c_str(), 10);
    return is;
  }
  // bitwise
  BigInt operator&(const BigInt &that) const {
    BigInt result;
    mpz_and(result.value, value, that.value);
    return result;
  }
  BigInt operator|(const BigInt &that) const {
    BigInt result;
    mpz_ior(result.value, value, that.value);
    return result;
  }
  BigInt operator^(const BigInt &that) const {
    BigInt result;
    mpz_xor(result.value, value, that.value);
    return result;
  }
  BigInt &operator&=(const BigInt &that) {
    mpz_and(value, value, that.value);
    return *this;
  }
  BigInt &operator|=(const BigInt &that) {
    mpz_ior(value, value, that.value);
    return *this;
  }
  BigInt &operator^=(const BigInt &that) {
    mpz_xor(value, value, that.value);
    return *this;
  }
};
