#pragma once

#include "bigint.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

class BigFraction {
  BigInt numerator;
  BigInt denominator;
  void reduce() {
    BigInt g = gcd(numerator, denominator);
    numerator = numerator / g;
    denominator = denominator / g;
  }

public:
  BigFraction() : denominator(BigInt(1)) {}
  BigFraction(const BigFraction &that) = default;
  BigFraction& operator=(const BigFraction &that) {
    if (this != &that) {
      numerator = that.numerator;
      denominator = that.denominator;
    }
    return *this;
  }
  explicit BigFraction(const BigInt &numerator)
      : numerator(numerator), denominator(BigInt(1)) {}

  explicit BigFraction(std::string &input) {
    std::string::size_type pos = input.find('.');
    if (pos == std::string::npos) {
      numerator = BigInt(input);
      denominator = BigInt(1);
    } else {
      input.replace(pos, 1, "");
      numerator = BigInt(input);
      denominator = BigInt(std::string("1") + std::string(input.size()-pos, '0'));
      reduce();
    }
  }
  // io
  friend std::ostream &operator<<(std::ostream &os, const BigFraction &f) {
    if (f.denominator == BigInt(1)) {
      os << f.numerator;
      return os;
    }
    std::stringstream temp;
    temp << f.numerator * BigInt("100000000000000000000") / f.denominator;
    std::string s = temp.str();
    s.insert(s.size() - 20, 1, '.');
    while (s[s.size() - 1] == '0')
      s.pop_back();
    os << s;
    return os;
  }
  // calculation
  friend BigFraction operator+(const BigFraction& a,const BigFraction& b) {
    BigFraction result;
    result.numerator = a.numerator * b.denominator + b.numerator * a.denominator;
    result.denominator = a.denominator * b.denominator;
    result.reduce();
    return result;
  }
  friend BigFraction operator-(const BigFraction& a,const BigFraction& b) {
    BigFraction result;
    result.numerator = a.numerator * b.denominator - b.numerator * a.denominator;
    result.denominator = a.denominator * b.denominator;
    result.reduce();
    return result;
  }
  friend BigFraction operator*(const BigFraction& a,const BigFraction& b) {
    BigFraction result;
    result.numerator = a.numerator * b.numerator;
    result.denominator = a.denominator * b.denominator;
    result.reduce();
    return result;
  }
  friend BigFraction operator/(const BigFraction& a,const BigFraction& b) {
    BigFraction result;
    result.numerator = a.numerator * b.denominator;
    result.denominator = a.denominator * b.numerator;
    result.reduce();
    return result;
  }
  BigFraction operator+=(const BigFraction &that) {
    *this = *this + that;
    return *this;
  }
  BigFraction operator-=(const BigFraction &that) {
    *this = *this - that;
    return *this;
  }
  BigFraction operator*=(const BigFraction &that) {
    *this = *this * that;
    return *this;
  }
  BigFraction operator/=(const BigFraction &that) {
    *this = *this / that;
    return *this;
  }
  // unary
  BigFraction &operator+() { return *this; }
  BigFraction operator-() const {
    BigFraction result;
    result.numerator = -numerator;
    result.denominator = denominator;
    return result;
  }
  // comparison
  bool operator<(const BigFraction &that) const {
    return numerator * that.denominator < that.numerator * denominator;
  }
  bool operator>(const BigFraction &that) const {
    return numerator * that.denominator > that.numerator * denominator;
  }
  bool operator==(const BigFraction &that) const {
    return numerator * that.denominator == that.numerator * denominator;
  }
};
