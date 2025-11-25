#include "int2048.h"

namespace sjtu {

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
  // std::cerr << "Reading int2048 from string: " << num << std::endl;
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
  // std::cerr << "Adding int2048 values" << std::endl;
  // std::cerr << a.to_string() << " + " << b.to_string() << std::endl;
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
  int carry = 0;
  size_t maxSize = std::max(a.s.size(), b.s.size());
  for (size_t i = 0; i < maxSize || carry; ++i) {
    int x = carry;
    if (i < a.s.size()) x += a.s[i];
    if (i < b.s.size()) x += b.s[i];
    result.s.push_back(x % int2048::BASE);
    carry = x / int2048::BASE;
  }
  
  result.delete_leading_zeros();
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
  int borrow = 0;
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
  if (n == 1) return;

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
  if (a.sign == 0 || b.sign == 0) return int2048(0);

  int2048 result;
  result.sign = a.sign * b.sign;

  size_t len = 1;
  while (len < a.s.size() + b.s.size()) len <<= 1;

  std::vector<std::complex<long double>> fft_a(a.s.begin(), a.s.end());
  std::vector<std::complex<long double>> fft_b(b.s.begin(), b.s.end());
  fft_a.resize(len);
  fft_b.resize(len);

  FFT(fft_a, false);
  FFT(fft_b, false);

  for (size_t i = 0; i < len; ++i) fft_a[i] *= fft_b[i];

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
  if (len >= num.s.size()) return result;

  result.s.assign(num.s.begin() + len, num.s.end());
  result.delete_leading_zeros();
  if (!result.s.empty()) {
    result.sign = 1;
  }
  return result;
}

int2048 get_low(const int2048 &num, size_t len) {
  int2048 result(0);
  if (num.s.empty() || len == 0) return result;

  result.s.assign(num.s.begin(), num.s.begin() + std::min(len, num.s.size()));
  result.delete_leading_zeros();
  if (!result.s.empty()) {
    result.sign = 1;
  }
  return result;
}

int2048 shift_left(const int2048 &num, size_t k) {
  if (num.sign == 0 || k == 0) return int2048(0);
  int2048 result;
  result.sign = num.sign;
  result.s.assign(k, 0);
  result.s.insert(result.s.end(), num.s.begin(), num.s.end());
  return result;
}

int2048 mul_short(const int2048 &a, int k) {
  if (a.sign == 0 || k == 0) return int2048(0);

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
  res.sign = res.s.empty() ? 0 : 1;
  return res;
}

std::pair<int2048, int2048> basic_divide(const int2048 &a, const int2048 &b) {
  int2048 remainder;
  int2048 quotient;
  quotient.s.assign(a.s.size(), 0);

  for (int i = (int)a.s.size() - 1; i >= 0; --i) {
    remainder.s.insert(remainder.s.begin(), a.s[i]);
    remainder.delete_leading_zeros();
    remainder.sign = remainder.s.empty() ? 0 : 1;

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
  if (a < b) return std::make_pair(int2048(0), a);
  if (a.s.size() <= (b.s.size() << 1)) return basic_divide(a, b);

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
  if (b.sign == 0) throw std::runtime_error("division by zero");
  if (a.sign == 0) return int2048(0);

  int result_sign = a.sign * b.sign;
  a.sign = 1;
  int2048 abs_b = b;
  abs_b.sign = 1;
  if (a < abs_b) {
    if (result_sign == -1) {
      return int2048(-1);
    } else {
      return int2048(0);
    }
  }

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
  if (b.sign == 0) throw std::runtime_error("modulo by zero");
  if (a.sign == 0) return int2048(0);
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
  // std::cerr << "Outputting int2048 value" << std::endl;
  // std::cerr << "x.sign: " << x.sign << std::endl;
  // std::cerr << "x.s size: " << x.s.size() << std::endl;
  if (x.sign == -1) out << '-';
  if (x.sign == 0) {
    out << '0';
    return out;
  }
  out << x.s.back();
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
  for (size_t i = 0; i < a.s.size(); ++i)
    if (a.s[i] != b.s[i]) return false;
  return true;
}

bool operator!=(const int2048 &a, const int2048 &b) {
  return !(a == b);
}

bool operator<(const int2048 &a, const int2048 &b) {
  if (a.sign != b.sign) return a.sign < b.sign;
  if (a.sign == 0) return false;
  if (a.s.size() != b.s.size())
    return (a.sign == 1) ? (a.s.size() < b.s.size())
                         : (a.s.size() > b.s.size());
  for (int i = (int)a.s.size() - 1; i >= 0; --i) {
    if (a.s[i] != b.s[i])
      return (a.sign == 1) ? (a.s[i] < b.s[i]) : (a.s[i] > b.s[i]);
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
