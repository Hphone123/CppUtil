#pragma once

#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

#include "String.hpp"

namespace CppUtil
{

template <uint8_t beforeDec = 16, uint8_t afterDec = 16> class Fixed
{
  static constexpr uint16_t bitsTotal = beforeDec + afterDec;
  using base_t                        = std::conditional_t<
    bitsTotal <= 8, uint8_t,
    std::conditional_t<
      bitsTotal <= 16, uint16_t,
      std::conditional_t<
        bitsTotal <= 32, uint32_t,
        std::conditional_t<bitsTotal <= 64, uint64_t, std::conditional_t<bitsTotal <= 128, unsigned __int128, void>>>>>;
  using s_base_t = std::conditional_t<
    bitsTotal <= 8, int16_t,
    std::conditional_t<bitsTotal <= 16, int32_t,
                       std::conditional_t<bitsTotal <= 32, int64_t,
                                          std::conditional_t<bitsTotal <= 64, __int128,
                                                             std::conditional_t<bitsTotal <= 128, __int128, void>>>>>;

  using float_t = std::conditional_t<beforeDec + afterDec <= 23, float, double>;

  static base_t constexpr BM_BEFORE_DEC = ((1ULL << beforeDec) - 1) << afterDec;
  static base_t constexpr BM_AFTER_DEC  = (1ULL << afterDec) - 1;
  static base_t constexpr BM_ALL_BITS   = (1ULL << (afterDec + beforeDec)) - 1;

  static_assert(std::is_arithmetic_v<base_t> && !std::is_floating_point_v<base_t> && std::is_unsigned_v<base_t>,
                "'_using' must be an unsigned integer!");
  static_assert(sizeof(base_t) * 8 >= beforeDec + afterDec,
                "'_using' must be big enough to fit '_beforeDec' and '_afterDec'!");

  bool   sign;
  base_t value;

public:
  static constexpr base_t BEFORE_DEC_MAX = (1ULL << beforeDec) - 1;
  static constexpr base_t VALUE_MAX      = (1ULL << bitsTotal) - 1;

  enum round_dir_e
  {
    ROUND_DIR_AUTO,
    ROUND_DIR_UP,
    ROUND_DIR_DOWN
  };

  /**
   * @brief Rounds this Fixed
   * 
   * @tparam dir The direction to round to
   * @return const s_base_t The integer representation of this Fixed
   * @note `dir` must be known on compile time. To choose dir on runtime, use `Fixed::round(round_dir_e dir);`.
   */
  template <Fixed::round_dir_e dir = ROUND_DIR_AUTO> const s_base_t round() const
  {
    const s_base_t sign = this->sign ? -1 : 1;

    const base_t   UNSIGNED_ROUND_DOWN = (this->value & BM_BEFORE_DEC) >> afterDec;
    const s_base_t SIGNED_ROUND_DOWN   = sign * UNSIGNED_ROUND_DOWN;

    const base_t   UNSIGNED_ROUND_UP = UNSIGNED_ROUND_DOWN + 1;
    const s_base_t SIGNED_ROUND_UP   = sign * UNSIGNED_ROUND_UP;

    if constexpr (dir == ROUND_DIR_DOWN)
    {
      return SIGNED_ROUND_DOWN;
    }
    else if constexpr (dir == ROUND_DIR_UP)
    {
      if (this->value & BM_AFTER_DEC) //? Check if any bit after the decimal is set to avoid rounding XXX.0 to XXX+1
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
    else
    {
      if ((this->value & BM_AFTER_DEC) &
          (1ULL << (beforeDec - 1))) //? Check if the highest bit after the decimal is set -> XXX.5 -> Round up
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
  };

  /**
   * @brief Rounds this Fixed
   * 
   * @param dir The direction to round to
   * @return const s_base_t The integer representation of this Fixed
   * @note If `dir` is known on compile time, consider using `Fixed::round<round_dir_e dir>()` for performance.
   */
  const s_base_t round(round_dir_e dir) const
  {
    const s_base_t sign = this->sign ? -1 : 1;

    const base_t   UNSIGNED_ROUND_DOWN = (this->value & BM_BEFORE_DEC) >> afterDec;
    const s_base_t SIGNED_ROUND_DOWN   = sign * UNSIGNED_ROUND_DOWN;

    const base_t   UNSIGNED_ROUND_UP = UNSIGNED_ROUND_DOWN + 1;
    const s_base_t SIGNED_ROUND_UP   = sign * UNSIGNED_ROUND_UP;

    if (dir == ROUND_DIR_DOWN)
    {
      return SIGNED_ROUND_DOWN;
    }
    else if (dir == ROUND_DIR_UP)
    {
      if (this->value & BM_AFTER_DEC) //? Check for XXX.0 (otherwise XXX.0 becomes XXX + 1)
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
    else
    {
      if ((this->value & BM_AFTER_DEC) & (1ULL << (beforeDec - 1)))
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
  };

  Fixed() : sign(false), value((base_t)(0ULL)){};

  /**
   * @brief Construct a new Fixed object
   * 
   * @param f The float to be constructed from
   * @attention Will 'stick' to the highest / lowest value possible 
   */
  Fixed(float f)
  {
    static constexpr uint32_t BM_FLOAT_EXP = (((1UL << 8) - 1) << 23);
    static constexpr uint32_t BM_FLOAT_MAN = (1UL << 23) - 1;

    const auto tmp = *(uint32_t *)&f;

    this->sign = tmp & (1UL << 31);

    if ((this->sign && -f > BEFORE_DEC_MAX) || (!this->sign && f > BEFORE_DEC_MAX))
    {
      this->value = VALUE_MAX;
      return;
    }

    if (!(tmp & ((1ULL << 31) - 1)))
    {
      this->value = 0UL;
      return;
    }

    const int8_t exp = ((tmp & BM_FLOAT_EXP) >> 23) - 127;
    const auto   man = (tmp & BM_FLOAT_MAN) + (1UL << 23);

    const int8_t VAL_MAN_SHIFT = (afterDec - 23) + exp;

    if (VAL_MAN_SHIFT == 0)
    {
      this->value = (man & BM_ALL_BITS);
    }
    else if (VAL_MAN_SHIFT > 0)
    {
      this->value = (man << VAL_MAN_SHIFT) & BM_ALL_BITS;
    }
    else
    {
      this->value = (man >> -VAL_MAN_SHIFT) & BM_ALL_BITS;
    }
  };

  Fixed(double d)
  {
    static constexpr uint64_t BM_FLOAT_EXP = (((1ULL << 11) - 1) << 52);
    static constexpr uint64_t BM_FLOAT_MAN = (1ULL << 52) - 1;

    const auto tmp = *(uint64_t *)&d;

    this->sign = tmp & (1ULL << 63);

    if ((this->sign && -d > BEFORE_DEC_MAX) || (!this->sign && d > BEFORE_DEC_MAX))
    {
      this->value = VALUE_MAX;
      return;
    }

    if (!(tmp & ((1ULL << 63) - 1)))
    {
      this->value = 0ULL;
      return;
    }

    const int8_t exp = ((tmp & BM_FLOAT_EXP) >> 52) - 1023;
    const auto   man = (tmp & BM_FLOAT_MAN) + (1ULL << 52);

    const int8_t VAL_MAN_SHIFT = (afterDec - 52) + exp;

    if (VAL_MAN_SHIFT == 0)
    {
      this->value = (man & BM_ALL_BITS);
    }
    else if (VAL_MAN_SHIFT > 0)
    {
      this->value = (man << VAL_MAN_SHIFT) & BM_ALL_BITS;
    }
    else
    {
      this->value = (man >> -VAL_MAN_SHIFT) & BM_ALL_BITS;
    }
  };

  template <typename U                                                                              = base_t,
            typename std::enable_if_t<std::is_same_v<U, base_t> && beforeDec + afterDec <= 23, int> = 0>
  operator float() const
  {
    static constexpr uint32_t BM_MANTISSA = (1UL << 23) - 1;

    uint32_t tmp = (sign << 31);
    for (uint8_t i = (beforeDec + afterDec) - 1; i > 0; i--)
    {
      if (this->value & (1ULL << i))
      {
        const uint8_t exp = (uint8_t)(127 + (i - afterDec));
        tmp += exp << 23;
        const int8_t shift = (int8_t)(i - 23);
        if (shift < 0)
        {
          tmp += (this->value << -shift) & BM_MANTISSA;
        }
        else
        {
          tmp += (this->value >> shift) & BM_MANTISSA;
        }
        return *(float *)&tmp;
      }
    }
    return 0.0f;
  };

  operator double() const
  {
    static constexpr uint64_t BM_MANTISSA = (1UL << 52) - 1;

    uint64_t tmp = ((uint64_t)this->sign << 63);
    for (uint8_t i = (beforeDec + afterDec) - 1; i > 0; i--)
    {
      if (this->value & (1ULL << i))
      {
        const uint8_t exp = (uint8_t)(1023 + (i - afterDec));
        tmp += ((uint64_t)exp << 52);
        const int8_t shift = (int8_t)(i - 52);
        if (shift < 0)
        {
          tmp += (this->value << -shift) & BM_MANTISSA;
        }
        else
        {
          tmp += (this->value >> shift) & BM_MANTISSA;
        }
        return *(double *)&tmp;
      }
    }
    return 0.0f;
  };

  template <uint8_t base = 10> String to_string() const
  {
    static_assert(base <= 36, "Cannot use Base >36!");

    String res_before = "";
    String res_after  = "";

    base_t before = (this->value & BM_BEFORE_DEC) >> afterDec;

    if (!before)
    {
      res_before = "0";
    }

    while (before)
    {
      const char c = (char)('0' + before % base);
      if (c > '9')
      {
        res_before = String((char)(c + 7)) + res_before; //? Skip ':' - '@'
      }
      else
      {
        res_before = String(c) + res_before;
      }
      before /= base;
    }

    base_t after = this->value & BM_AFTER_DEC;

    if (after)
    {
      res_after = ".";
    }

    while (after)
    {
      after *= base;
      res_after = res_after + (char)('0' + (after / (1ULL << afterDec)));
      after %= (1ULL << afterDec);
    }

    if (this->sign)
    {
      return String('-') + res_before + res_after;
    }
    else
    {
      return res_before + res_after;
    };
  };

  Fixed& operator+=(const Fixed<beforeDec, afterDec>& other)
  {
    //ToDo: Implement
  }
};

template <uint8_t before, uint8_t after>
Fixed<before, after> operator+(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a += b;
  return a;
}
} // namespace CppUtil