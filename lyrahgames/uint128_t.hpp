#pragma once
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

namespace lyrahgames {

struct uint128_t {
  uint64_t low{};
  uint64_t high{};

  constexpr uint128_t() = default;
  constexpr uint128_t(uint64_t l, uint64_t h) : low{l}, high{h} {}
  constexpr uint128_t(std::unsigned_integral auto x) : low{x}, high{0} {}

  constexpr explicit operator bool() { return low || high; }
};

constexpr uint128_t operator~(uint128_t x) { return {~x.low, ~x.high}; }
constexpr uint128_t operator&(uint128_t x, uint128_t y) {
  return {x.low & y.low, x.high & y.high};
}
constexpr uint128_t operator|(uint128_t x, uint128_t y) {
  return {x.low | y.low, x.high | y.high};
}
constexpr uint128_t operator^(uint128_t x, uint128_t y) {
  return {x.low ^ y.low, x.high ^ y.high};
}
constexpr uint128_t operator<<(uint128_t x, size_t s) {
  if (s > 64u) return {0u, x.low << (s - 64u)};
  return {x.low << s, (x.high << s) | (x.low >> (64u - s))};
}
constexpr uint128_t operator>>(uint128_t x, size_t s) {
  if (s > 64u) return {x.high >> (s - 64u), 0u};
  return {(x.low >> s) | (x.high << (64u - s)), (x.high >> s)};
}

constexpr bool operator==(uint128_t x, uint128_t y) {
  return (x.high == y.high) && (x.low == y.low);
}
constexpr bool operator!=(uint128_t x, uint128_t y) {
  return (x.high != y.high) || (x.low != y.low);
}
constexpr bool operator<(uint128_t x, uint128_t y) {
  return (x.high < y.high) || ((x.high == y.high) && (x.low < y.low));
}
constexpr bool operator>(uint128_t x, uint128_t y) {
  return (x.high > y.high) || ((x.high == y.high) && (x.low > y.low));
}
constexpr bool operator<=(uint128_t x, uint128_t y) { return !(x > y); }
constexpr bool operator>=(uint128_t x, uint128_t y) { return !(x < y); }

constexpr uint128_t operator+(uint128_t x, uint128_t y) {
  const uint64_t low = x.low + y.low;
  return {low, x.high + y.high + (low < x.low)};
}

constexpr uint128_t operator-(uint128_t x, uint128_t y) {
  const uint64_t low = x.low - y.low;
  return {low, x.high - y.high - (low > x.low)};
}

constexpr size_t bits(uint128_t x) {
  size_t result = 0;
  if (x.high) {
    result = 64;
    uint64_t tmp = x.high;
    while (tmp) {
      tmp >>= 1;
      ++result;
    }
  } else {
    uint64_t tmp = x.low;
    while (tmp) {
      tmp >>= 1;
      ++result;
    }
  }
  return result;
}

constexpr uint128_t operator*(uint128_t x, uint128_t y) {
  const auto x00 = x.low & ((uint64_t{1} << 32) - 1);
  const auto y00 = y.low & ((uint64_t{1} << 32) - 1);
  const auto x01 = x.low >> 32;
  const auto y01 = y.low >> 32;

  const auto x01y00 = x01 * y00;
  const auto x00y01 = x00 * y01;
  const auto mid = x01y00 + x00y01;

  const auto x00y00 = x00 * y00;
  const auto lower = x00y00 + (mid << 32);

  return {lower,  //
          x.high * y.low + x.low * y.high + x01 * y01 + (mid >> 32) +
              (uint64_t(mid < x00y01) << 32) + uint64_t(lower < x00y00)};
}

constexpr std::pair<uint128_t, uint128_t> divmod(uint128_t x, uint128_t y) {
  uint128_t div{};
  uint128_t mod{};
  for (auto i = bits(x); i > 0; --i) {
    div = div << 1u;
    mod = mod << 1u;
    if ((x >> (i - 1u)) & 1u) {
      mod = mod + 1u;
    }
    if (mod >= y) {
      mod = mod - y;
      div = div + 1u;
    }
  }
  return {div, mod};
}

inline std::ostream& operator<<(std::ostream& os, uint128_t x) {
  os << std::setfill('0') << std::setbase(16) << std::setw(16) << x.high << ' '
     << std::setw(16) << x.low << std::setbase(10) << std::setfill(' ');
  std::string number{};
  while (x) {
    const auto [div, mod] = divmod(x, 10u);
    number += char('0' + mod.low);
    x = div;
  }
  for (size_t i = 0; i < number.size() / 2; ++i) {
    std::swap(number[i], number[number.size() - 1 - i]);
  }
  return os << std::setw(50) << number;
}

}  // namespace lyrahgames