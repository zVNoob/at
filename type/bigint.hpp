#pragma once

#include <gmp.h>
#include <istream>
#include <ostream>

class BigInt {
  mpz_t value;

public:
  BigInt() { mpz_init(value); }
  BigInt(const BigInt &that) { mpz_init_set(value, that.value); }
  BigInt &operator=(const BigInt &that) {
    mpz_set(value, that.value);
    return *this;
  }

  explicit BigInt(const std::string &str) { mpz_init_set_str(value, str.c_str(), 10); }
  explicit BigInt(long n) { mpz_init_set_si(value, n); }
  ~BigInt() {
      mpz_clear(value);
  }
  long to_long() const { return mpz_get_si(value); }
  // calculation
  friend BigInt operator+(const BigInt& a,const BigInt& b){
    BigInt result;
    mpz_add(result.value, a.value, b.value);
    return result;
  }
  friend BigInt operator-(const BigInt& a,const BigInt& b) {
    BigInt result;
    mpz_sub(result.value, a.value, b.value);
    return result;
  }
  friend BigInt operator*(const BigInt& a,const BigInt& b) {
    BigInt result;
    mpz_mul(result.value, a.value, b.value);
    return result;
  }
  friend BigInt operator/(const BigInt& a,const BigInt& b) {
    BigInt result;
    mpz_div(result.value, a.value, b.value);
    return result;
  }
  friend BigInt operator%(const BigInt& a,const BigInt& b) {
    BigInt result;
    mpz_mod(result.value, a.value, b.value);
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
  BigInt &operator+() { return *this; }
  // io
  friend std::ostream &operator<<(std::ostream &os, const BigInt &that) {
    char* s = mpz_get_str(nullptr, 10, that.value);
    os << s;
    free(s);
    return os;
  }
  friend std::istream &operator>>(std::istream &is, BigInt &that) {
    std::string str;
    is >> str;
    mpz_set_str(that.value, str.c_str(), 10);
    return is;
  }
  friend BigInt gcd(BigInt a, BigInt b) {
    BigInt r;
    mpz_gcd(r.value, a.value, b.value);
    return r;
  }

};
