#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

namespace sjtu {
class int2048 {
public:
  const static int BASE = 10000;
  const static int WIDTH = 4;
  constexpr static long double PI = 3.141592653589793238462643383279502884L;

  std::vector<int> s;
  int sign;

  // constructors
  int2048();
  int2048(long long);
  int2048(const std::string &);
  int2048(const int2048 &);

  void read(const std::string &);
  void print();
  void delete_leading_zeros();
  std::string to_string() const;
  double to_double() const;

  int2048 &add(const int2048 &);
  int2048 &minus(const int2048 &);

  int2048 operator+() const;
  int2048 operator-() const;
  int2048 &operator=(const int2048 &);

  int2048 &operator+=(const int2048 &);
  int2048 &operator-=(const int2048 &);

  int2048 &operator*=(const int2048 &);
  int2048 &operator/=(const int2048 &);
  int2048 &operator%=(const int2048 &);

  friend int2048 add(int2048, const int2048 &);
  friend int2048 minus(int2048, const int2048 &);

  friend int2048 operator+(int2048, const int2048 &);
  friend int2048 operator-(int2048, const int2048 &);
  friend int2048 operator*(int2048, const int2048 &);
  friend int2048 operator/(int2048, const int2048 &);
  friend int2048 operator%(int2048, const int2048 &);

  friend void FFT(std::vector<std::complex<long double>> &, bool);

  friend std::istream &operator>>(std::istream &, int2048 &);
  friend std::ostream &operator<<(std::ostream &, const int2048 &);

  friend bool operator==(const int2048 &, const int2048 &);
  friend bool operator!=(const int2048 &, const int2048 &);
  friend bool operator<(const int2048 &, const int2048 &);
  friend bool operator>(const int2048 &, const int2048 &);
  friend bool operator<=(const int2048 &, const int2048 &);
  friend bool operator>=(const int2048 &, const int2048 &);
};

// helper functions
int2048 get_high(const int2048 &, size_t);
int2048 get_low(const int2048 &, size_t);
int2048 shift_left(const int2048 &, size_t);
int2048 mul_short(const int2048 &, int);
std::pair<int2048, int2048> basic_divide(const int2048 &, const int2048 &);
std::pair<int2048, int2048> divide(const int2048 &, const int2048 &);

} // namespace sjtu

#endif // SJTU_BIGINTEGER