#pragma once

#include <cstdint>
#include <ostream>
#include <istream>

namespace urara {
using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;
using int64 = std::int64_t;
using uint64 = std::uint64_t;
using int128 = __int128_t;
using uint128 = __uint128_t;

using float32 = float;
using float64 = double;
using float80 = long double;
using float128 = __float128;

constexpr uint128 cstr_to_uint128(const char* str, uint32 idx = 0, uint128 result = 0) {
  return str[idx] == '\0'
    ? result
    : ('0' <= str[idx] && str[idx] <= '9'
        ? cstr_to_uint128(str, idx + 1, result * 10 + str[idx] - '0')
        : cstr_to_uint128(str, idx + 1, result));
}

constexpr int128 cstr_to_int128(const char* str, uint32 idx = 0, int128 result = 0) {
  return str[idx] == '\0'
    ? result
    : (str[idx] == '-'
        ? -cstr_to_uint128(str, idx + 1)
        : ('0' <= str[idx] && str[idx] <= '9'
            ? cstr_to_uint128(str, idx)
            : cstr_to_int128(str, idx + 1, result)));
}

std::ostream& operator << (std::ostream& os, const uint128 &v) {
  constexpr uint32 digits = 18;
  constexpr uint64 ten18 = uint64(1e18);

  uint128 n = v;
  char str[40];

  uint32 pos = sizeof(str);
  str[--pos] = '\0';
  do {
    uint64 r = n % ten18;
    n /= ten18;
    if (n) {
      for (uint32 i = 0; i < digits; ++i) {
        str[--pos] = '0' + r % 10;
        r /= 10;
      }
    } else {
      do {
        str[--pos] = '0' + r % 10;
        r /= 10;
      } while (r);
    }
  } while (n);
  os << std::string(str + pos);
  return os;
}

std::istream& operator >> (std::istream &is, uint128 &v) {
  std::string s;
  is >> s;
  v = cstr_to_uint128(s.c_str());
  return is;
}

std::ostream& operator << (std::ostream& os, const int128 &v) {
  if (v < 0) {
    os << '-' << uint128(v);
  } else {
    os << uint128(v);
  }
  return os;
}

std::istream& operator >> (std::istream &is, int128 &v) {
  std::string s;
  is >> s;
  v = cstr_to_int128(s.c_str());
  return is;
}

constexpr uint128 operator "" _128u (const char *s) {
  return cstr_to_uint128(str);
}

constexpr int128 operator "" _128 (const char *s) {
  return cstr_to_int128(str);
}

}
