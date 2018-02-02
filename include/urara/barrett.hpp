#ifndef URARA_BARRETT_HPP
#define URARA_BARRETT_HPP

#include <urara/types.hpp>
#include <urara/bits.hpp>

namespace urara {

template<typename word, typename dword>
struct FastDivision {
  constexpr static int word_bits = sizeof(word) * 8;

  FastDivision() = default;
  FastDivision(word n): m(n) {
    if (n == 1) {
      x = 1;
      shamt = 0;
    } else {
      shamt = bits::ilog2(n - 1) + word_bits;
      x = ((dword(1) << shamt) + n - 1) / n;
    }
  }

  friend word operator / (const word &n, const FastDivision &d) {
    return dword(n) * d.x >> d.shamt;
  }

  friend word operator % (const word &n, const FastDivision &d) {
    return n - n / d * d.m;
  }

private:
  int shamt;
  word m, x;
};

template<typename word>
struct ExactDivisor {
  ExactDivisor() = default;
  ExactDivisor(const word &n): t(word(-1) / n), i(mul_inv(n)) {}

  constexpr static word mul_inv(word n, int e = 6, word x = 1) {
    return !e ? x : mul_inv(n, e - 1, x * (2 - x * n));
  }

  friend word operator / (const word &n, const ExactDivisor &d) {
    return n * d.i;
  }

  bool divide(const word &n) const {
    return n * i <= t;
  }

private:
  word t, i;
};

using FastDiv32 = FastDivision<uint32, uint64>;
using FastDiv64 = FastDivision<uint64, uint128>;
using ExactDiv32 = ExactDivisor<uint32>;
using ExactDiv64 = ExactDivisor<uint64>;

}

#endif
