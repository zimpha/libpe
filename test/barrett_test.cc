#include <urara/barrett.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <cmath>
#include <omp.h>

using namespace urara;

template <typename func_t, typename ...Args>
void timer(func_t func, Args... args) {
  auto old_fmtflags = std::cout.flags();
  auto old_precision = std::cout.precision();

  auto time_beg = omp_get_wtime();
  func(std::forward<Args>(args)...);
  auto time_end = omp_get_wtime();

  std::cout.precision(4);
  std::cout << std::fixed;
  std::cout << "elapsed time: " << time_end - time_beg << " sec." << std::endl;

  std::cout.precision(old_precision);
  std::cout.flags(old_fmtflags);
}

inline uint64 mod64_32_small(uint64 a, uint32 b) {
  uint32 q, r;
  __asm__ (
    "divl\t%4"
    : "=a"(q), "=d"(r)
    : "0"(uint32(a)), "1"(uint32(a >> 32)), "rm"(b)
  );
  return r;
}

void test_fast_div() {
  std::cout << "Test for n! % mod" << std::endl;
  auto fact_slow = [](uint32 n, uint32 mod) {
    uint32 ret = 1;
    for (uint32 i = 1; i <= n; ++i) {
      ret = uint64(ret) * i % mod;
    }
    std::cout << "result = " << ret << ", ";
  };

  auto fact_slow_asm = [](uint32 n, uint32 mod) {
    uint32 ret = 1;
    for (uint32 i = 1; i <= n; ++i) {
      ret = mod64_32_small(uint64(ret) * i,  mod);
    }
    std::cout << "result = " << ret << ", ";
  };

  auto fact_fast = [](uint32 n, uint32 mod) {
    auto fd = FastDiv64(mod);
    uint32 ret = 1;
    for (uint32 i = 1; i <= n; ++i) {
      ret = uint64(ret) * i % fd;
    }
    std::cout << "result = " << ret << ", ";
  };

  std::vector<std::pair<uint32, uint32>> tests = {
    {1e8 + 6, 1e8 + 7},
    {1e8 + 6, 1e9 + 6},
    {5e8, 1e9 + 7}
  };
  for (auto &&test: tests) {
    uint32 n = test.first;
    uint32 mod = test.second;
    std::cout << "  Test # n = " << n << ", mod = " << mod << std::endl;
    std::cout << "    fact_slow    : ";
    timer(fact_slow, n, mod);
    std::cout << "    fact_slow_asm: ";
    timer(fact_slow_asm, n, mod);
    std::cout << "    fact_fast    : ";
    timer(fact_fast, n, mod);
  }
  std::cout << std::endl;
}

void test_exact_div() {
  std::cout << "Test for counting number of odd primes between l and r" << std::endl;
  constexpr int n = 2e5;
  std::vector<ExactDiv64> div64(n);
  std::vector<ExactDiv32> div32(n);
  for (int i = 1; i <= n; ++i) {
    div64[i] = ExactDiv64(i);
    div32[i] = ExactDiv32(i);
  }
  auto is_odd_prime_slow_32 = [](uint32 n) {
    uint32 sqrt_n = sqrt(n);
    for (uint32 i = 3; i <= sqrt_n; i += 2) {
      if (n % i == 0) return false;
    }
    return true;
  };

  auto is_odd_prime_slow_64 = [](uint64 n) {
    uint32 sqrt_n = sqrtl(n);
    for (uint32 i = 3; i <= sqrt_n; i += 2) {
      if (n % i == 0) return false;
    }
    return true;
  };

  auto is_odd_prime_fast_32 = [&](uint32 n) {
    uint32 sqrt_n = sqrt(n);
    for (uint32 i = 3; i <= sqrt_n; i += 2) {
      if (div32[i].divide(n)) return false;
    }
    return true;
  };

  auto is_odd_prime_fast_64 = [&](uint64 n) {
    uint32 sqrt_n = sqrtl(n);
    for (uint32 i = 3; i <= sqrt_n; i += 2) {
      if (div64[i].divide(n)) return false;
    }
    return true;
  };

  auto prime_count_slow_32 = [&](uint32 l, uint32 r) {
    int ret = 0;
    for (uint32 i = l; i < r; ++i) {
      ret += is_odd_prime_slow_32(i);
    }
    std::cout << "result = " << ret << ", ";
  };

  auto prime_count_slow_64 = [&](uint64 l, uint64 r) {
    int ret = 0;
    for (uint64 i = l; i < r; ++i) {
      ret += is_odd_prime_slow_64(i);
    }
    std::cout << "result = " << ret << ", ";
  };

  auto prime_count_fast_32 = [&](uint32 l, uint32 r) {
    int ret = 0;
    for (uint32 i = l; i < r; ++i) {
      ret += is_odd_prime_fast_32(i);
    }
    std::cout << "result = " << ret << ", ";
  };

  auto prime_count_fast_64 = [&](uint64 l, uint64 r) {
    int ret = 0;
    for (uint64 i = l; i < r; ++i) {
      ret += is_odd_prime_fast_64(i);
    }
    std::cout << "result = " << ret << ", ";
  };

  std::vector<std::pair<uint32, uint32>> tests_32 = {
    {3, 1e6 + 3}, {1e9 + 1, 1e9 + 2e5 + 1}
  };
  std::vector<std::pair<uint64, uint64>> tests_64 = {
    {1e10 + 1, 1e10 + 2e5 + 1}, {2e10 + 1, 2e10 + 2e5 + 1}
  };

  for (auto &&test: tests_32) {
    uint32 l = test.first, r = test.second;
    std::cout << "  Test # l = " << l << ", r = " << r << std::endl;
    std::cout << "    prime_count_slow_32: ";
    timer(prime_count_slow_32, l, r);
    std::cout << "    prime_count_fast_32: ";
    timer(prime_count_fast_32, l, r);
  }
  for (auto &&test: tests_64) {
    uint64 l = test.first, r = test.second;
    std::cout << "  Test # l = " << l << ", r = " << r << std::endl;
    std::cout << "    prime_count_slow_64: ";
    timer(prime_count_slow_64, l, r);
    std::cout << "    prime_count_fast_64: ";
    timer(prime_count_fast_64, l, r);
  }
  std::cout << std::endl;
}

int main() {
  test_fast_div();
  test_exact_div();
  return 0;
}
