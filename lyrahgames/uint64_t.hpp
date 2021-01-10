#pragma once
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

namespace lyrahgames {
struct uint64 {
  uint32_t low{};
  uint32_t high{};

  constexpr uint64() = default;
  constexpr uint64(uint32_t l, uint32_t h) : low{l}, high{h} {}
  constexpr uint64(std::unsigned_integral auto x) : low{x}, high{0} {}

  constexpr explicit operator bool() { return low || high; }
};

constexpr uint64 operator~(uint64 x) { return {~x.low, ~x.high}; }
constexpr uint64 operator&(uint64 x, uint64 y) {
  return {x.low & y.low, x.high & y.high};
}
constexpr uint64 operator|(uint64 x, uint64 y) {
  return {x.low | y.low, x.high | y.high};
}
constexpr uint64 operator^(uint64 x, uint64 y) {
  return {x.low ^ y.low, x.high ^ y.high};
}
constexpr uint64 operator<<(uint64 x, size_t s) {
  if (s > 32u) return {0u, x.low << (s - 32u)};
  return {x.low << s, (x.high << s) | (x.low >> (32u - s))};
}
constexpr uint64 operator>>(uint64 x, size_t s) {
  if (s > 32u) return {x.high >> (s - 32u), 0u};
  return {(x.low >> s) | (x.high << (32u - s)), (x.high >> s)};
}

constexpr bool operator==(uint64 x, uint64 y) {
  return (x.high == y.high) && (x.low == y.low);
}
constexpr bool operator!=(uint64 x, uint64 y) {
  return (x.high != y.high) || (x.low != y.low);
}
constexpr bool operator<(uint64 x, uint64 y) {
  return (x.high < y.high) || ((x.high == y.high) && (x.low < y.low));
}
constexpr bool operator>(uint64 x, uint64 y) {
  return (x.high > y.high) || ((x.high == y.high) && (x.low > y.low));
}
constexpr bool operator<=(uint64 x, uint64 y) { return !(x > y); }
constexpr bool operator>=(uint64 x, uint64 y) { return !(x < y); }

constexpr uint64 operator+(uint64 x, uint64 y) {
  const uint32_t low = x.low + y.low;
  return {low, x.high + y.high + (low < x.low)};
}

constexpr uint64 operator-(uint64 x, uint64 y) {
  const uint32_t low = x.low - y.low;
  return {low, x.high - y.high - (low > x.low)};
}

constexpr size_t bits(uint64 x) {
  size_t result = 0;
  if (x.high) {
    result = 32;
    uint32_t tmp = x.high;
    while (tmp) {
      tmp >>= 1;
      ++result;
    }
  } else {
    uint32_t tmp = x.low;
    while (tmp) {
      tmp >>= 1;
      ++result;
    }
  }
  return result;
}

constexpr uint64 operator*(uint64 x, uint64 y) {
  const auto x00 = x.low & ((uint32_t{1} << 16) - 1);
  const auto y00 = y.low & ((uint32_t{1} << 16) - 1);
  const auto x01 = x.low >> 16;
  const auto y01 = y.low >> 16;

  const auto x01y00 = x01 * y00;
  const auto x00y01 = x00 * y01;
  const auto mid = x01y00 + x00y01;

  const auto x00y00 = x00 * y00;
  const auto lower = x00y00 + (mid << 16);

  return {lower,  //
          x.high * y.low + x.low * y.high + x01 * y01 + (mid >> 16) +
              (uint32_t(mid < x00y01) << 16) + uint32_t(lower < x00y00)};
}

constexpr std::pair<uint64, uint64> divmod(uint64 x, uint64 y) {
  uint64 div{};
  uint64 mod{};
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

inline std::ostream& operator<<(std::ostream& os, uint64 x) {
  os << std::setfill('0') << std::setbase(16) << std::setw(8) << x.high << ' '
     << std::setw(8) << x.low << std::setbase(10) << std::setfill(' ');
  std::string number{};
  while (x) {
    const auto [div, mod] = divmod(x, 10u);
    number += char('0' + mod.low);
    x = div;
  }
  for (size_t i = 0; i < number.size() / 2; ++i) {
    std::swap(number[i], number[number.size() - 1 - i]);
  }
  return os << std::setw(30) << number;
}

}  // namespace lyrahgames