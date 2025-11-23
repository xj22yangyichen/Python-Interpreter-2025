#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Integer 1:
// 实现一个有符号的大整数类，只需支持简单的加减

// Integer 2:
// 实现一个有符号的大整数类，支持加减乘除，并重载相关运算符

// 请不要使用除了以下头文件之外的其它头文件
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

// 请不要使用 using namespace std;

namespace sjtu {
  class int2048 {
    // todo
  public:
    // 如果要修改 BASE，还需要修改 print 中的代码
    const static int BASE = 10000;
    const static int WIDTH = 4;
    constexpr static long double PI = 3.141592653589793238462643383279502884L;
    std::vector<int> s;
    int sign; // 1 正数 -1 负数 0 零

    // 构造函数
    int2048();
    int2048(long long);
    int2048(const std::string &);
    int2048(const int2048 &);

    // 以下给定函数的形式参数类型仅供参考，可自行选择使用常量引用或者不使用引用
    // 如果需要，可以自行增加其他所需的函数
    // ===================================
    // Integer1
    // ===================================

    // 读入一个大整数
    void read(const std::string &);
    // 输出储存的大整数，无需换行
    void print();
    // 删除前导零
    void delete_leading_zeros();
    // 转化为字符串
    std::string to_string() const;
    // 转化为 double
    double to_double() const;

    // 加上一个大整数
    int2048 &add(const int2048 &);
    // 返回两个大整数之和
    friend int2048 add(int2048, const int2048 &);

    // 减去一个大整数
    int2048 &minus(const int2048 &);
    // 返回两个大整数之差
    friend int2048 minus(int2048, const int2048 &);

    // ===================================
    // Integer2
    // ===================================

    int2048 operator+() const;
    int2048 operator-() const;

    int2048 &operator=(const int2048 &);

    int2048 &operator+=(const int2048 &);
    friend int2048 operator+(int2048, const int2048 &);

    int2048 &operator-=(const int2048 &);
    friend int2048 operator-(int2048, const int2048 &);

    // 用 FFT
    int2048 &operator*=(const int2048 &);
    friend int2048 operator*(int2048, const int2048 &);
    friend void FFT(std::vector<std::complex<long double>> &, bool);

    // 用分治除法
    int2048 &operator/=(const int2048 &);
    friend int2048 operator/(int2048, const int2048 &);

    int2048 &operator%=(const int2048 &);
    friend int2048 operator%(int2048, const int2048 &);

    friend std::istream &operator>>(std::istream &, int2048 &);
    friend std::ostream &operator<<(std::ostream &, const int2048 &);

    friend bool operator==(const int2048 &, const int2048 &);
    friend bool operator!=(const int2048 &, const int2048 &);
    friend bool operator<(const int2048 &, const int2048 &);
    friend bool operator>(const int2048 &, const int2048 &);
    friend bool operator<=(const int2048 &, const int2048 &);
    friend bool operator>=(const int2048 &, const int2048 &);
  };

  int2048::int2048() : sign(0) {
    s.clear();
  }

  int2048::int2048(long long num) {
    s.clear();
    if (num == 0) {
      sign = 0;
      return;
    }
    if (num < 0) {
      sign = -1;
      num = -num;
    } else {
      sign = 1;
    }
    while (num > 0) {
      s.emplace_back(num % BASE);
      num /= BASE;
    }
  }

  int2048::int2048(const std::string &num) {
    read(num);
  }

  int2048::int2048(const int2048 &num) {
    s = num.s;
    sign = num.sign;
  }

  void int2048::read(const std::string &num) {
    int len = num.length();
    s.clear();
    if (len == 0 || (len == 1 && num[0] == '0') || (len == 2 && num == "-0")) {
      sign = 0;
      return;
    }
    int start = 0;
    if (num[0] == '-') {
      sign = -1;
      start = 1;
    } else if (num[0] == '+') {
      sign = 1;
      start = 1;
    } else {
      sign = 1;
    }

    for (int i = len - 1; i >= start; i -= WIDTH) {
      int x = 0;
      for (int j = std::max(start, i - WIDTH + 1); j <= i; ++j) {
        x = x * 10 + (num[j] - '0');
      }
      s.emplace_back(x);
    }
    delete_leading_zeros();
  }

  void int2048::print() {
    if (sign == -1) putchar('-');
    if (sign == 0) {
      putchar('0');
      return;
    }
    printf("%d", s.back());
    for (int i = (int)s.size() - 2; i >= 0; --i) {
      printf("%04d", s[i]);
    }
  }

  void int2048::delete_leading_zeros() {
    while (!s.empty() && s.back() == 0) {
      s.pop_back();
    }
    if (s.empty()) {
      sign = 0;
    }
  }

  std::string int2048::to_string() const {
    if (sign == 0) return "0";
    std::string result;
    if (sign == -1) result += '-';
    result += std::to_string(s.back());
    for (int i = (int)s.size() - 2; i >= 0; --i) {
      std::string part = std::to_string(s[i]);
      result += std::string(WIDTH - part.length(), '0') + part;
    }
    return result;
  }

  double int2048::to_double() const {
    if (sign == 0) return 0.0;
    double result = 0.0;
    double base_pow = 1.0;
    for (size_t i = 0; i < s.size(); ++i) {
      result += s[i] * base_pow;
      base_pow *= BASE;
    }
    if (sign == -1) result = -result;
    return result;
  }

  int2048 add(int2048 a, const int2048 &b) {
    if (a.sign == 0) return b;
    if (b.sign == 0) return a;
    if (a.sign != b.sign) {
      if (a.sign == -1) {
        a.sign = 1;
        return minus(b, a);
      } else {
        int2048 temp_b(b);
        temp_b.sign = 1;
        return minus(a, temp_b);
      }
    }

    int2048 result;
    result.sign = a.sign;
    int carry = 0; // 进位
    size_t maxSize = std::max(a.s.size(), b.s.size());
    for (size_t i = 0; i < maxSize || carry; ++i) {
      int x = carry;
      if (i < a.s.size()) {
        x += a.s[i];
      }
      if (i < b.s.size()) {
        x += b.s[i];
      }
      result.s.push_back(x % int2048::BASE);
      carry = x / int2048::BASE;
    }
    return result;
  }

  int2048 &int2048::add(const int2048 &b) {
    *this = sjtu::add(*this, b);
    return *this;
  }

  int2048 minus(int2048 a, const int2048 &b) {
    if (b.sign == 0) return a;
    if (a.sign == 0) {
      int2048 result(b);
      result.sign = -b.sign;
      return result;
    }
    if (a.sign != b.sign) {
      int2048 temp_b(b);
      temp_b.sign = -temp_b.sign;
      return add(a, temp_b);
    }

    if (a.sign == -1) {
      int2048 temp_a(a);
      int2048 temp_b(b);
      temp_a.sign = 1;
      temp_b.sign = 1;
      return minus(temp_b, temp_a);
    }

    if (a < b) {
      int2048 result = minus(b, a);
      result.sign = -1;
      return result;
    }

    int2048 result;
    result.sign = 1;
    int borrow = 0; // 借位
    for (size_t i = 0; i < a.s.size(); ++i) {
      int x = a.s[i] - borrow;
      if (i < b.s.size()) x -= b.s[i];
      if (x < 0) {
        x += int2048::BASE;
        borrow = 1;
      } else {
        borrow = 0;
      }
      result.s.emplace_back(x);
    }
    result.delete_leading_zeros();
    return result;
  }

  int2048 &int2048::minus(const int2048 &b) {
    *this = sjtu::minus(*this, b);
    return *this;
  }

  int2048 int2048::operator+() const {
    return *this;
  }

  int2048 int2048::operator-() const {
    int2048 result(*this);
    result.sign = -result.sign;
    return result;
  }

  int2048 &int2048::operator=(const int2048 &b) {
    s = b.s;
    sign = b.sign;
    return *this;
  }

  int2048 &int2048::operator+=(const int2048 &b) {
    *this = sjtu::add(*this, b);
    return *this;
  }

  int2048 operator+(int2048 a, const int2048 &b) {
    return sjtu::add(a, b);
  }

  int2048 &int2048::operator-=(const int2048 &b) {
    *this = sjtu::minus(*this, b);
    return *this;
  }

  int2048 operator-(int2048 a, const int2048 &b) {
    return sjtu::minus(a, b);
  }

  void FFT(std::vector<std::complex<long double>> &a, bool invert) {
    int n = a.size();
    if (n == 1) {
      return;
    }
    std::vector<std::complex<long double>> a0(n >> 1), a1(n >> 1);
    for (int i = 0; 2 * i < n; ++i) {
      a0[i] = a[2 * i];
      a1[i] = a[2 * i + 1];
    }

    FFT(a0, invert);
    FFT(a1, invert);

    long double angle = 2 * int2048::PI / n * (invert ? 1 : -1);
    std::complex<long double> omega(1), delta(cosl(angle), sinl(angle));
    for (int i = 0; 2 * i < n; ++i) {
      a[i] = a0[i] + omega * a1[i];
      a[i + n / 2] = a0[i] - omega * a1[i];
      omega *= delta;
    }
  }

  int2048 operator*(int2048 a, const int2048 &b) {
    if (a.sign == 0 || b.sign == 0) {
      return int2048(0);
    }
    int2048 result;
    result.sign = a.sign * b.sign;

    size_t len = 1;
    while (len < a.s.size() + b.s.size()) {
      len <<= 1;
    }
    std::vector<std::complex<long double>> fft_a(a.s.begin(), a.s.end());
    std::vector<std::complex<long double>> fft_b(b.s.begin(), b.s.end());
    fft_a.resize(len);
    fft_b.resize(len);

    FFT(fft_a, false);
    FFT(fft_b, false);
    for (size_t i = 0; i < len; ++i) {
      fft_a[i] *= fft_b[i];
    }
    FFT(fft_a, true);

    result.s.resize(len);
    long long carry = 0;
    for (size_t i = 0; i < len; ++i) {
      long long x = (long long)(fft_a[i].real() / len + 0.5) + carry;
      result.s[i] = x % int2048::BASE;
      carry = x / int2048::BASE;
    }
    while (carry) {
      result.s.push_back(carry % int2048::BASE);
      carry /= int2048::BASE;
    }
    result.delete_leading_zeros();
    return result;
  }

  int2048 &int2048::operator*=(const int2048 &b) {
    *this = (*this) * b;
    return *this;
  }

  int2048 get_high(const int2048 &num, size_t len) {
    int2048 result(0);
    if (len >= num.s.size()) {
      return result;
    }
    result.s.assign(num.s.begin() + len, num.s.end());
    result.delete_leading_zeros();
    if (!result.s.empty()) {
      result.sign = 1;
    }
    return result;
  }

  int2048 get_low(const int2048 &num, size_t len) {
    int2048 result(0);
    if (num.s.empty() || len == 0) {
      return result;
    }
    result.s.assign(num.s.begin(), num.s.begin() + std::min(len, num.s.size()));
    result.delete_leading_zeros();
    if (!result.s.empty()) {
      result.sign = 1;
    }
    return result;
  }

  int2048 shift_left(const int2048 &num, size_t k) {
    if (num.sign == 0 || k == 0) {
      return int2048(0);
    }
    int2048 result;
    result.sign = num.sign;
    result.s.assign(k, 0);
    result.s.insert(result.s.end(), num.s.begin(), num.s.end());
    return result;
  }

  int2048 mul_short(const int2048 &a, int k) {
    if (a.sign == 0 || k == 0) {
      return int2048(0);
    }
    long long carry = 0;
    int2048 res;
    res.s.resize(a.s.size());
    for (size_t i = 0; i < a.s.size(); ++i) {
      long long cur = (long long)a.s[i] * (long long)k + carry;
      res.s[i] = (int)(cur % int2048::BASE);
      carry = cur / int2048::BASE;
    }
    while (carry) {
      res.s.push_back((int)(carry % int2048::BASE));
      carry /= int2048::BASE;
    }
    res.delete_leading_zeros();
    if (!res.s.empty()) {
      res.sign = 1;
    } else {
      res.sign = 0;
    }
    return res;
  }

  std::pair<int2048, int2048> basic_divide(const int2048 &a, const int2048 &b) {
    int2048 remainder;
    int2048 quotient;
    quotient.s.assign(a.s.size(), 0);

    // std::cerr << "Basic dividing " << a << " by " << b << std::endl;

    for (int i = (int)a.s.size() - 1; i >= 0; --i) {
      remainder.s.insert(remainder.s.begin(), a.s[i]);
      remainder.delete_leading_zeros();
      if (remainder.s.empty()) {
        remainder.sign = 0;
      } else {
        remainder.sign = 1;
      }
      if (remainder < b) {
        quotient.s[i] = 0;
        continue;
      }
      int left = 0, right = int2048::BASE - 1, q = 0;
      while (left <= right) {
        int mid = (left + right) >> 1;
        int2048 prod = mul_short(b, mid);
        if (prod <= remainder) {
          q = mid;
          left = mid + 1;
        } else {
          right = mid - 1;
        }
      }
      quotient.s[i] = q;
      remainder = remainder - mul_short(b, q);
    }

    quotient.delete_leading_zeros();
    remainder.delete_leading_zeros();
    quotient.sign = quotient.s.empty() ? 0 : 1;
    remainder.sign = remainder.s.empty() ? 0 : 1;
    return {quotient, remainder};
  }

  std::pair<int2048, int2048> divide(const int2048 &a, const int2048 &b) {
    if (a < b) {
      return std::make_pair(int2048(0), a);
    }
    // 用二分除法处理小规模数据
    if (a.s.size() <= (b.s.size() << 1)) {
      return basic_divide(a, b);
    }

    int m = (int)a.s.size() >> 1;
    int2048 high_a = get_high(a, m);
    int2048 low_a = get_low(a, m);

    auto [quotient_high, remainder_high] = divide(high_a, b);
    int2048 temp = shift_left(remainder_high, m) + low_a;
    auto [quotient_low, remainder_low] = divide(temp, b);

    int2048 quotient = shift_left(quotient_high, m) + quotient_low;
    return std::make_pair(quotient, remainder_low);
  }

  int2048 operator/(int2048 a, const int2048 &b) {
    if (b.sign == 0) {
      throw std::runtime_error("division by zero");
    }
    if (a.sign == 0) {
      return int2048(0);
    }

    int result_sign = a.sign * b.sign;
    a.sign = 1;
    int2048 abs_b = b;
    abs_b.sign = 1;
    
    auto [quotient, remainder] = divide(a, abs_b);
    quotient.sign = result_sign;
    if (result_sign == -1 && remainder.sign != 0) {
      quotient = quotient - int2048(1);
    }
    return quotient;
  }

  int2048 &int2048::operator/=(const int2048 &b) {
    *this = (*this) / b;
    return *this;
  }

  int2048 operator%(int2048 a, const int2048 &b) {
    if (b.sign == 0) {
      throw std::runtime_error("modulo by zero");
    }
    if (a.sign == 0) {
      return int2048(0);
    }

    return a - (a / b) * b;
  }

  int2048 &int2048::operator%=(const int2048 &b) {
    *this = (*this) % b;
    return *this;
  }

  std::istream &operator>>(std::istream &in, int2048 &x) {
    std::string str;
    in >> str;
    x.read(str);
    return in;
  }

  std::ostream &operator<<(std::ostream &out, const int2048 &x) {
    if (x.sign == -1) out << '-';
    if (x.sign == 0) {
      out << '0';
      return out;
    }
    // std::cerr << "Outputting int2048: ";
    out << x.s.back();
    // std::cerr << "Outputting int2048: ";
    for (int i = (int)x.s.size() - 2; i >= 0; --i) {
      out.width(int2048::WIDTH);
      out.fill('0');
      out << x.s[i];
    }
    return out;
  }

  bool operator==(const int2048 &a, const int2048 &b) {
    if (a.sign != b.sign) return false;
    if (a.s.size() != b.s.size()) return false;
    for (size_t i = 0; i < a.s.size(); ++i) {
      if (a.s[i] != b.s[i]) return false;
    }
    return true;
  }

  bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
  }

  bool operator<(const int2048 &a, const int2048 &b) {
    if (a.sign != b.sign) return a.sign < b.sign;
    if (a.sign == 0) return false;
    if (a.s.size() != b.s.size()) {
      return (a.sign == 1) ? (a.s.size() < b.s.size()) : (a.s.size() > b.s.size());
    }
    for (int i = (int)a.s.size() - 1; i >= 0; --i) {
      if (a.s[i] != b.s[i]) {
        return (a.sign == 1) ? (a.s[i] < b.s[i]) : (a.s[i] > b.s[i]);
      }
    }
    return false;
  }

  bool operator>(const int2048 &a, const int2048 &b) {
    return b < a;
  }

  bool operator<=(const int2048 &a, const int2048 &b) {
    return !(a > b);
  }

  bool operator>=(const int2048 &a, const int2048 &b) {
    return !(a < b);
  }

} // namespace sjtu

#endif
