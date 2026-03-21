#pragma once

#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
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

  template <Fixed::round_dir_e direction = ROUND_DIR_AUTO> const base_t round() const
  {
    if constexpr (direction == ROUND_DIR_DOWN)
    {
      return (this->value & BM_BEFORE_DEC) >> afterDec;
    }
    else if constexpr (direction == ROUND_DIR_UP)
    {
      if (this->value & BM_AFTER_DEC)
      {
        return ((this->value & BM_BEFORE_DEC) >> afterDec) + 1;
      }
      else
      {
        return (this->value & BM_BEFORE_DEC) >> afterDec;
      }
    }
    else
    {
      if ((this->value & BM_AFTER_DEC) & (1ULL << (beforeDec - 1)))
      {
        return ((this->value & BM_BEFORE_DEC) >> afterDec) + 1;
      }
      else
      {
        return (this->value & BM_BEFORE_DEC) >> afterDec;
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

    const int8_t exp = ((tmp & BM_FLOAT_EXP) >> 52) - 1023;
    const auto   man = (tmp & BM_FLOAT_MAN) + (1ULL << 52);

    const int8_t VAL_MAN_SHIFT = (24 - (afterDec + exp));

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
          tmp += (this->value << -shift) & ((1UL << 23) - 1);
        }
        else
        {
          tmp += (this->value >> shift) & ((1UL << 23) - 1);
        }
        return *(float *)&tmp;
      }
    }
    return 0.0f;
  }
};