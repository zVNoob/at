#pragma once

#include "bigint.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class BigFraction {
  BigInt numerator;
  BigInt denominator;
  BigInt gcd(BigInt a, BigInt b) {
    if (a < b)
      std::swap(a, b);
    while (b != BigInt(0)) {
      a = a % b;
      std::swap(a, b);
    }
    return a;
  }
  void reduce() {
    if (denominator == BigInt(0)) {
      throw std::runtime_error("division by zero");
    }
    BigInt g = gcd(numerator, denominator);
    numerator = numerator / g;
    denominator = denominator / g;
  }

public:
  BigFraction() {}
  BigFraction(const BigInt &numerator)
      : numerator(numerator), denominator(BigInt(1)) {}
  BigFraction(std::string &input) {
    std::string::size_type pos = input.find('.');
    if (pos == std::string::npos) {
      numerator = BigInt(input);
      denominator = BigInt(1);
    } else {
      input.replace(pos, 1, "");
      numerator = BigInt(input);
      denominator = BigInt(std::string("1") + std::string(pos, '0'));
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
  BigFraction operator+(const BigFraction &that) const {
    BigFraction result;
    result.numerator =
        numerator * that.denominator + that.numerator * denominator;
    result.denominator = denominator * that.denominator;
    result.reduce();
    return result;
  }
  BigFraction operator-(const BigFraction &that) const {
    BigFraction result;
    result.numerator =
        numerator * that.denominator - that.numerator * denominator;
    result.denominator = denominator * that.denominator;
    result.reduce();
    return result;
  }
  BigFraction operator*(const BigFraction &that) const {
    BigFraction result;
    result.numerator = numerator * that.numerator;
    result.denominator = denominator * that.denominator;
    result.reduce();
    return result;
  }
  BigFraction operator/(const BigFraction &that) const {
    BigFraction result;
    result.numerator = numerator * that.denominator;
    result.denominator = denominator * that.numerator;
    result.reduce();
    return result;
  }
  BigFraction operator+=(const BigFraction &that) {
    *this = *this + that;
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
};
