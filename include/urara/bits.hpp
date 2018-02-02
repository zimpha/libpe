#ifndef URARA_BITS_HPP
#define URARA_BITS_HPP

#include <urara/types.hpp>

namespace urara {

// be careful with x = 0
struct bits {
  bits() = delete;

  constexpr static uint32 popcount(uint32 x) {
    return __builtin_popcount(x);
  }

  constexpr static uint32 popcount(uint64 x) {
    return __builtin_popcountll(x);
  }

  constexpr static uint32 popcount(uint128 x) {
    return popcount(uint64(x)) + popcount(uint64(x >> 64));
  }

  constexpr static uint32 clz(uint32 x) {
    return __builtin_clz(x);
  }

  constexpr static uint32 clz(uint64 x) {
    return __builtin_clzll(x);
  }

  constexpr static uint32 clz(uint128 x) {
    return uint64(x >> 64) ? clz(uint64(x >> 64)) : 64 + clz(uint64(x & UINT64_MAX));
  }

  constexpr static uint32 ctz(uint32 x) {
    return __builtin_ctz(x);
  }

  constexpr static uint32 ctz(uint64 x) {
    return __builtin_ctzll(x);
  }

  constexpr static uint32 ctz(uint128 x) {
    return uint64(x) ? ctz(uint64(x)) : 64 + ctz(uint64(x >> 64));
  }

  constexpr static uint32 ilog2(uint32 x) {
    return 31 - clz(x);
  }

  constexpr static uint32 ilog2(uint64 x) {
    return 63 - clz(x);
  }

  constexpr static uint32 ilog2(uint128 x) {
    return 127 - clz(x);
  }
};

}

#endif
