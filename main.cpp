#include <cassert>
#include <random>
//
#include <lyrahgames/uint128_t.hpp>
#include <lyrahgames/uint64_t.hpp>

using namespace std;
using namespace lyrahgames;

constexpr bool operator==(uint64_t r, uint64 m) {
  return (m.high == uint32_t(r >> 32)) && (m.low == uint32_t(r));
}

int main() {
  mt19937 rng{random_device{}()};
  uniform_int_distribution<uint32_t> distribution{};

  for (size_t i = 0; i < 100'000; ++i) {
    uint64 x{distribution(rng), distribution(rng)};
    uint64 y{distribution(rng), distribution(rng)};
    uint64_t a = (uint64_t(x.high) << 32) | uint64_t(x.low);
    uint64_t b = (uint64_t(y.high) << 32) | uint64_t(y.low);
    assert(a == x);
    assert(b == y);
    assert(a + b == x + y);
    assert(a - b == x - y);
    assert(a * b == x * y);
    const auto [div, mod] = divmod(x, y);
    if (!(a / b == div)) {
      cout << i << ' ' << a << ' ' << b << ' ' << a / b << ' ' << a % b << ' '
           << div << '\n';
      return -1;
    }
    // assert(a % b == mod);
    if (!(a % b == mod)) {
      cout << i << ' ' << a << ' ' << b << ' ' << a / b << ' ' << a % b << '\n'
           << setbase(16) << a << ' ' << b << '\n'
           << x << '\n'
           << y << '\n'
           << div << '\n'
           << mod << '\n';
      return -1;
    }
  }

  // uint128_t n(1, 0);
  // for (size_t i = 2; i < 100; ++i) {
  //   cout << n << setw(10) << bits(n) << '\n';
  //   n = n * i;
  //   // n = n << 1u;
  // }
  // for (int i = 0; i < 16; ++i) {
  //   n = n * 3u;
  //   cout << n << '\n';
  // }

  // while (true) {
  //   n = n + (0x01ull << 54);
  //   cout << n << '\n';
  // }
}