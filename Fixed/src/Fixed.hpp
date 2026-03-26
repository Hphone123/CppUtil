#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "String.hpp"

namespace CppUtil
{
template <typename T> struct unsigned_integer_traits
{
  static constexpr bool defined = false;
};
template <> struct unsigned_integer_traits<uint8_t>
{
  static constexpr bool     defined = true;
  static constexpr uint64_t n_bits  = 8;
};
template <> struct unsigned_integer_traits<uint16_t>
{
  static constexpr bool     defined = true;
  static constexpr uint64_t n_bits  = 16;
};
template <> struct unsigned_integer_traits<uint32_t>
{
  static constexpr bool     defined = true;
  static constexpr uint64_t n_bits  = 32;
};
template <> struct unsigned_integer_traits<uint64_t>
{
  static constexpr bool     defined = true;
  static constexpr uint64_t n_bits  = 64;
};
template <> struct unsigned_integer_traits<unsigned __int128>
{
  static constexpr bool     defined = true;
  static constexpr uint64_t n_bits  = 128;
};

/**
 * @brief Gets the signed equivalent to `T`
 * 
 * @tparam T The type to get the signed equivalent to
 */
template <typename T> struct get_signed
{
  static_assert(unsigned_integer_traits<T>::defined,
                "'T' must be unsigned-integer-like and define 'unsigned_integer_traits'!");
};
template <> struct get_signed<uint8_t>
{
  using type = int8_t;
};
template <> struct get_signed<uint16_t>
{
  using type = int16_t;
};
template <> struct get_signed<uint32_t>
{
  using type = int32_t;
};
template <> struct get_signed<uint64_t>
{
  using type = int64_t;
};
template <> struct get_signed<unsigned __int128>
{
  using type = __int128;
};
template <typename T> using get_signed_t = typename get_signed<T>::type;

//* Unsigned integer operators
template <typename, typename = void> struct has_unsigned_integer_operators : std::false_type
{
};

template <typename T>
struct has_unsigned_integer_operators<
  T, std::void_t<decltype(T{0}), decltype(T{1}),

                 decltype(std::declval<T>() + std::declval<T>()), decltype(std::declval<T>() - std::declval<T>()),
                 decltype(std::declval<T>() * std::declval<T>()), decltype(std::declval<T>() / std::declval<T>()),
                 decltype(std::declval<T>() % std::declval<T>()),

                 decltype(std::declval<T&>() += std::declval<T>()), decltype(std::declval<T&>() -= std::declval<T>()),
                 decltype(std::declval<T&>() *= std::declval<T>()), decltype(std::declval<T&>() /= std::declval<T>()),
                 decltype(std::declval<T&>() %= std::declval<T>()),

                 decltype(std::declval<T>() << std::declval<unsigned>()),
                 decltype(std::declval<T>() >> std::declval<unsigned>()),

                 decltype(std::declval<T>() == std::declval<T>()), decltype(std::declval<T>() != std::declval<T>()),
                 decltype(std::declval<T>() < std::declval<T>()), decltype(std::declval<T>() <= std::declval<T>()),
                 decltype(std::declval<T>() > std::declval<T>()), decltype(std::declval<T>() >= std::declval<T>())>>
    : std::true_type
{
};
template <typename T> using has_unsigned_integer_operators_v = typename has_unsigned_integer_operators<T>::value;

template <typename, typename = void> struct is_unsigned_integer_like : std::false_type
{
};
template <typename T>
struct is_unsigned_integer_like<
  T, std::enable_if<unsigned_integer_traits<T>::defined && has_unsigned_integer_operators_v<T>::value>> : std::true_type
{
};

//* No basic type large enough to fit the bits before / after
struct too_large_t
{
};
template <typename T> struct is_too_large_t : std::is_same<T, too_large_t>
{
};

template <uint64_t beforeDec, uint64_t afterDec, bool is_signed>
using base_t_default_t = typename std::conditional_t<
  (beforeDec + afterDec + is_signed) <= 8, uint8_t,
  std::conditional_t<(beforeDec + afterDec + is_signed) <= 16, uint16_t,
                     std::conditional_t<(beforeDec + afterDec + is_signed) <= 32, uint32_t,
                                        std::conditional_t<(beforeDec + afterDec + is_signed) <= 64, uint64_t,
                                                           std::conditional_t<(beforeDec + afterDec + is_signed) <= 128,
                                                                              unsigned __int128, too_large_t>>>>>;

/**
 * @brief A field-point class
 */
template <uint64_t beforeDec = 16, uint64_t afterDec = 15, bool is_signed = true,
          typename base_t = base_t_default_t<beforeDec, afterDec, is_signed>>
class Fixed
{
private:
  base_t _value;

public:
  using signed_base_t = get_signed_t<base_t>;

  static base_t constexpr BM_BEFORE_DEC = (((base_t)1 << beforeDec) - 1) << afterDec;
  static base_t constexpr BM_AFTER_DEC  = ((base_t)1 << afterDec) - 1;
  template <typename = std::enable_if<is_signed>>
  static base_t constexpr BM_SIGN           = is_signed ? ((base_t)1 << (beforeDec + afterDec)) : 0;
  static base_t constexpr BM_ALL_VALUE_BITS = ((beforeDec + afterDec) >= unsigned_integer_traits<base_t>::n_bits) ?
                                                (base_t)-1 :
                                                (((base_t)1 << (afterDec + beforeDec))) - 1;

  static_assert(!is_too_large_t<base_t>::value, "No primitive available fitting 'beforeDec + afterDec + is_signed'!");
  static_assert(
    std::is_integral_v<base_t> || is_unsigned_integer_like<base_t>::value,
    "'base_t' must either be a basic unsigned integer, or behave like one and define 'unsigned_integer_traits'!");
  static_assert(unsigned_integer_traits<base_t>::n_bits >= (beforeDec + afterDec + (is_signed ? 1 : 0)),
                "'base_t' must be large enough to fit all bits!");

  static constexpr base_t BEFORE_DEC_MAX = ((base_t)1 << beforeDec) - 1;
  static constexpr base_t VALUE_MAX      = BM_ALL_VALUE_BITS;

  enum round_dir_e
  {
    ROUND_DIR_AUTO,
    ROUND_DIR_UP,
    ROUND_DIR_DOWN
  };

  const constexpr bool sign() const
  {
    base_t x = BM_SIGN<>;
    x        = x + 0;
    if constexpr (!is_signed)
      return false;
    else
      return (this->_value & BM_SIGN<>) ? true : false;
  }

  void constexpr set_sign(bool sign)
  {
    static_assert(is_signed, "Cannot set sign on unsigned Fixed!");
    this->_value =
      (base_t)((base_t)(sign ? 1 : 0) << (beforeDec + afterDec)) + (base_t)(this->_value & BM_ALL_VALUE_BITS);
  }

  const constexpr base_t value() const
  {
    return this->_value & BM_ALL_VALUE_BITS;
  }

  void constexpr set_value(base_t value)
  {
    if constexpr (is_signed)
    {
      const base_t sign = (base_t)this->sign() << (beforeDec + afterDec);
      this->_value      = (value & this->BM_ALL_VALUE_BITS) + sign;
    }
    else
    {
      this->_value = (value & this->BM_ALL_VALUE_BITS);
    }
  }

  /**
   * @brief Rounds this Fixed
   * 
   * @tparam dir The direction to round to
   * @return const s_base_t The integer representation of this Fixed
   * @note `dir` must be known on compile time. To choose dir on runtime, use `Fixed::round(round_dir_e dir);`.
   */
  template <Fixed::round_dir_e dir = ROUND_DIR_AUTO> constexpr const signed_base_t round() const
  {
    const signed_base_t sign = this->sign() ? -1 : 1;

    const base_t        UNSIGNED_ROUND_DOWN = (this->value() >> afterDec);
    const signed_base_t SIGNED_ROUND_DOWN   = sign * UNSIGNED_ROUND_DOWN;

    const base_t        UNSIGNED_ROUND_UP = UNSIGNED_ROUND_DOWN + 1;
    const signed_base_t SIGNED_ROUND_UP   = sign * UNSIGNED_ROUND_UP;

    if constexpr (dir == ROUND_DIR_DOWN)
    {
      return SIGNED_ROUND_DOWN;
    }
    else if constexpr (dir == ROUND_DIR_UP)
    {
      if (this->_value & BM_AFTER_DEC) //? Check if any bit after the decimal is set to avoid rounding XXX.0 to XXX+1
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
    else if constexpr (dir == ROUND_DIR_AUTO)
    {
      if ((this->_value & BM_AFTER_DEC) &
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
  const signed_base_t round(round_dir_e dir = ROUND_DIR_AUTO) const
  {
    const signed_base_t sign = this->sign() ? -1 : 1;

    const base_t        UNSIGNED_ROUND_DOWN = (this->_value & BM_BEFORE_DEC) >> afterDec;
    const signed_base_t SIGNED_ROUND_DOWN   = sign * UNSIGNED_ROUND_DOWN;

    const base_t        UNSIGNED_ROUND_UP = UNSIGNED_ROUND_DOWN + 1;
    const signed_base_t SIGNED_ROUND_UP   = sign * UNSIGNED_ROUND_UP;

    if (dir == ROUND_DIR_DOWN)
    {
      return SIGNED_ROUND_DOWN;
    }
    else if (dir == ROUND_DIR_UP)
    {
      if (this->_value & BM_AFTER_DEC) //? Check for XXX.0 (otherwise XXX.0 becomes XXX + 1)
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
      if ((this->_value & BM_AFTER_DEC) & (1ULL << (beforeDec - 1)))
      {
        return SIGNED_ROUND_UP;
      }
      else
      {
        return SIGNED_ROUND_DOWN;
      }
    }
  };

  constexpr const Fixed<beforeDec, afterDec, is_signed> abs() const
  {
    Fixed<beforeDec, afterDec, is_signed, base_t> res = Fixed<beforeDec, afterDec, is_signed, base_t>();
    res._value += this->_value & BM_ALL_VALUE_BITS;
    return res;
  }

  constexpr Fixed() : _value((base_t)0){};

  /**
   * @brief Construct a new Fixed object
   * 
   * @tparam allowNegativeOnUnsigned Wether to allow converting negative 'float'-values to an 'unsigned Fixed'. Must only be set on 'unsigned Fixed'.
   * @param f The float to be constructed from
   * @attention Will 'stick' to the highest / lowest value possible 
   */
  template <int8_t allowNegativeOnUnsigned = -1> constexpr Fixed(float f)
  {
    static_assert(!(!is_signed && allowNegativeOnUnsigned < 0),
                  "'float' to 'unsigned Fixed' conversion must specify behaviour for negative values!");

    constexpr uint32_t BM_FLOAT_EXP = (((1UL << 8) - 1) << 23);
    constexpr uint32_t BM_FLOAT_MAN = (1UL << 23) - 1;

    const auto tmp = *(uint32_t *)&f;

    if (is_signed)
    {
      this->set_sign(tmp & (1UL << 31));
    }
    else
    {
      if (tmp & (1UL << 31) && !allowNegativeOnUnsigned)
      {
        throw "Converting a negative 'float' into an 'unsigned Fixed' was disallowed by template parameter!";
      }
    }

    if ((this->sign() && -f > BEFORE_DEC_MAX) || (!this->sign() && f > BEFORE_DEC_MAX))
    {
      this->set_value(VALUE_MAX);
      return;
    }

    if (!(tmp & ((1ULL << 31) - 1)))
    {
      this->set_value(0UL);
      return;
    }

    const int8_t exp = ((tmp & BM_FLOAT_EXP) >> 23) - 127;
    const auto   man = (tmp & BM_FLOAT_MAN) + (1UL << 23);

    const int8_t VAL_MAN_SHIFT = (afterDec - 23) + exp;

    if (VAL_MAN_SHIFT == 0)
    {
      this->set_value(man & BM_ALL_VALUE_BITS);
    }
    else if (VAL_MAN_SHIFT > 0)
    {
      this->set_value((man << VAL_MAN_SHIFT) & BM_ALL_VALUE_BITS);
    }
    else
    {
      this->set_value((man >> -VAL_MAN_SHIFT) & BM_ALL_VALUE_BITS);
    }
  };

  template <int8_t allowNegativeOnUnsigned = -1> constexpr Fixed(double d)
  {
    static_assert(!(!is_signed && allowNegativeOnUnsigned < 0),
                  "'double' to 'unsigned Fixed' conversion must specify behaviour for negative values!");

    constexpr uint64_t BM_DOUBLE_EXP = (((1ULL << 11) - 1) << 52);
    constexpr uint64_t BM_DOUBLE_MAN = (1ULL << 52) - 1;

    const auto tmp = *(uint64_t *)&d;

    if (is_signed)
    {
      this->set_sign(tmp & (1UL << 63));
    }
    else
    {
      if (tmp & (1UL << 63) && !allowNegativeOnUnsigned)
      {
        throw "Converting a negative 'double' into an 'unsigned Fixed' was disallowed by template parameter!";
      }
    }

    if ((this->sign() && -d > BEFORE_DEC_MAX) || (!this->sign() && d > BEFORE_DEC_MAX))
    {
      this->set_value(VALUE_MAX);
      return;
    }

    if (!(tmp & ((1ULL << 63) - 1)))
    {
      this->set_value(0ULL);
      return;
    }

    const int8_t exp = ((tmp & BM_DOUBLE_EXP) >> 52) - 1023;
    const auto   man = (tmp & BM_DOUBLE_MAN) + (1ULL << 52);

    const int8_t VAL_MAN_SHIFT = (afterDec - 52) + exp;

    if (VAL_MAN_SHIFT == 0)
    {
      this->set_value(man);
    }
    else if (VAL_MAN_SHIFT > 0)
    {
      this->set_value(man << VAL_MAN_SHIFT);
    }
    else
    {
      this->set_value(man >> -VAL_MAN_SHIFT);
    }
  }

  /**
   * @brief Parse a Fixed from a String
   * 
   * @tparam base The numeric base
   * @param s The String to be parsed
   * @return Fixed
   */
  template <uint8_t base = 10, uint64_t bef = 64, uint64_t aft = 63, bool sig = true,
            typename b_t = base_t_default_t<bef, aft, sig>>
  static constexpr Fixed<bef, aft, sig, b_t> parse(const char * s, size_t len)
  {
    Fixed<bef, aft, sig> res    = Fixed<bef, aft, sig>();
    b_t                  before = 0, afterNumer = 0, after = 0, afterDenom = 1;
    if (s[0] == '-')
    {
      if (!is_signed)
      {
        throw "Unsigned Fixed cannot have a negative sign!";
      }
      res.set_sign(true);
    }

    bool encounteredDecimal = false;

    for (size_t i = res.sign() ? 1 : 0; i < len; i++)
    {
      char c = s[i];

      if (c == '.')
      {
        encounteredDecimal = true;
        continue;
      }

      if (c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
        throw "Invalid character!";

      constexpr char ASCII_NUMERIC_BOUNDS     = '/' + base;
      constexpr char ASCII_ALPHA_UPPER_BOUNDS = ASCII_NUMERIC_BOUNDS + 7;
      constexpr char ASCII_ALPHA_LOWER_BOUNDS = '`' + base - 10;

      uint8_t n = 0;

      if (base > 10)
      {
        if (c > ASCII_ALPHA_UPPER_BOUNDS && c > ASCII_ALPHA_LOWER_BOUNDS)
          throw "Character is outside of bounds for given base!";

        n = c - '0';

        if (n > '9')
        {
          if (n >= 'a')
            n &= 0b11011111;
          n -= 7;
        }
      }
      else
      {
        if (c > ASCII_NUMERIC_BOUNDS)
          throw "Character is outside of bounds for given base!";

        n = c - '0';
      }

      if (n >= base)
        throw "Character is outside base!";

      if (encounteredDecimal)
      {
        afterDenom *= base;
        afterNumer = (afterNumer * base) + n;
      }
      else
      {
        before *= base;
        before += n;
      }
    }

    for (size_t i = 0; i < aft; i++)
    {
      afterNumer *= 2;
      after <<= 1;
      if (afterNumer >= afterDenom)
      {
        after |= 1;
        afterNumer -= afterDenom;
      }
    }

    res.set_value((before << aft) + (after & res.BM_AFTER_DEC));
    return res;
  }

  template <uint64_t before, uint64_t after, bool sign, typename base>
  constexpr operator Fixed<before, after, sign, base>()
  {
    Fixed<before, after, sign, base> res = Fixed<before, after, sign, base>();
    if constexpr (sign)
    {
      res.set_sign(this->sign());
    }
    res.set_value((((this->_value & this->BM_BEFORE_DEC) >> afterDec) << after));

    if constexpr (before < beforeDec)
    {
      res.set_value(res.value() + ((this->_value & this->BM_AFTER_DEC) >> (beforeDec - before)));
    }
    else
    {
      res.set_value(res.value() + ((this->_value & this->BM_AFTER_DEC) << (beforeDec - before)));
    }

    return res;
  }

  template <typename U                                                                              = base_t,
            typename std::enable_if_t<std::is_same_v<U, base_t> && beforeDec + afterDec <= 23, int> = 0>
  constexpr operator float() const
  {
    constexpr uint32_t BM_MANTISSA = (1UL << 23) - 1;

    uint32_t tmp = (this->sign() << 31);
    for (uint8_t i = (beforeDec + afterDec) - 1; i > 0; i--)
    {
      if (this->_value & (1ULL << i))
      {
        const uint8_t exp = (uint8_t)(127 + (i - afterDec));
        tmp += exp << 23;
        const int8_t shift = (int8_t)(i - 23);
        if (shift < 0)
        {
          tmp += (this->_value << -shift) & BM_MANTISSA;
        }
        else
        {
          tmp += (this->_value >> shift) & BM_MANTISSA;
        }
        return *(float *)&tmp;
      }
    }
    return 0.0f;
  };

  constexpr operator double() const
  {
    constexpr uint64_t BM_MANTISSA = (1ULL << 52) - 1;

    uint64_t tmp = ((uint64_t)this->sign() << 63);
    for (uint8_t i = (beforeDec + afterDec) - 1; i > 0; i--)
    {
      if (this->_value & (1ULL << i))
      {
        const uint16_t exp = (uint16_t)(1023 + (i - afterDec));
        tmp += ((uint64_t)(exp & ((1UL << 11) - 1)) << 52);
        const int8_t shift = (int8_t)(i - 52);
        if (shift < 0)
        {
          tmp += ((uint64_t)this->_value << -shift) & BM_MANTISSA;
        }
        else
        {
          tmp += ((uint64_t)this->_value >> shift) & BM_MANTISSA;
        }
        return *(double *)&tmp;
      }
    }
    return 0.0;
  };

  template <uint8_t base = 10> String to_string() const
  {
    static_assert(base <= 36, "Cannot use Base > 36!");

    String res_before = "";
    String res_after  = "";

    base_t before = (this->value() & BM_BEFORE_DEC) >> afterDec;

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

    base_t after = this->value() & BM_AFTER_DEC;

    if (after)
    {
      res_after = ".";
    }

    while (after)
    {
      after *= base;
      res_after = res_after + (char)('0' + (after / ((base_t)1 << afterDec)));
      after %= ((base_t)1 << afterDec);
    }

    if (this->sign())
    {
      return String('-') + res_before + res_after;
    }
    else
    {
      return res_before + res_after;
    };
  };

  constexpr Fixed& operator+=(const Fixed<beforeDec, afterDec, is_signed>& other)
  {
    if (other.sign())
    {
      this -= other;
      return *this;
    }
    else
    {
      if (is_signed)
      {
        if (this->_value < other._value)
          this->set_sign(!this->sign());
      }

      this->set_value(this->value() + other.value());
      return *this;
    }
  };

  constexpr Fixed& operator-=(const Fixed<beforeDec, afterDec>& other)
  {
    if (other.sign)
    {
      this += other;
      return *this;
    }
    else
    {
      if (is_signed)
      {
        if (this->_value < other._value)
          this->set_sign(!this->sign());
      }

      this->set_value(this->value() - other.value());
      return *this;
    }
  };

  constexpr Fixed& operator*=(const Fixed<beforeDec, afterDec>& other)
  {
    if (other.sign)
      this->set_sign(!this->sign());

    this->set_value(this->value() * other.value());
    return *this;
  };

  constexpr Fixed& operator/=(const Fixed<beforeDec, afterDec>& other)
  {
    if (other.sign)
      this->set_sign(!this->sign());

    this->set_value(this->value() / other.value());
    return *this;
  };

  constexpr Fixed& operator%=(const Fixed<beforeDec, afterDec>& other)
  {
    if (other.sign)
      this->set_sign(!this->sign());

    this->set_value(this->value() % other.value());
    return *this;
  };
};

typedef Fixed<8, 7, true>   s_fp16;
typedef Fixed<16, 15, true> s_fp32;
typedef Fixed<32, 31, true> s_fp64;
typedef Fixed<64, 63, true> s_fp128;

typedef Fixed<8, 8, false>   u_fp16;
typedef Fixed<16, 16, false> u_fp32;
typedef Fixed<32, 32, false> u_fp64;
typedef Fixed<64, 64, false> u_fp128;

constexpr u_fp16 operator""_u_fp16(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<8, 8, false>::parse<10>(str, len);
}

constexpr u_fp32 operator""_u_fp32(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<16, 16, false>::parse<10>(str, len);
}

constexpr u_fp64 operator""_u_fp64(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<32, 32, false>::parse<10>(str, len);
}

constexpr u_fp128 operator""_u_fp128(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<64, 64, false>::parse<10>(str, len);
}

constexpr s_fp16 operator""_s_fp16(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<8, 7, true>::parse<10>(str, len);
}

constexpr s_fp32 operator""_s_fp32(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<16, 15, true>::parse<10>(str, len);
}

constexpr s_fp64 operator""_s_fp64(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<32, 31, true>::parse<10>(str, len);
}

constexpr s_fp128 operator""_s_fp128(const char * str)
{
  size_t len = 0;
  while (str[len] != '\0')
    len++;
  return Fixed<64, 63, true>::parse<10>(str, len);
}

template <uint64_t before, uint64_t after>
Fixed<before, after> constexpr operator+(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a += b;
  return a;
}

template <uint64_t before, uint64_t after>
Fixed<before, after> constexpr operator-(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a -= b;
  return a;
}

template <uint64_t before, uint64_t after>
Fixed<before, after> constexpr operator*(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a *= b;
  return a;
}

template <uint64_t before, uint64_t after>
Fixed<before, after> constexpr operator/(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a /= b;
  return a;
}

template <uint64_t before, uint64_t after>
Fixed<before, after> constexpr operator%(Fixed<before, after> a, const Fixed<before, after>& b)
{
  a %= b;
  return a;
}

template <uint64_t before, uint64_t after>
bool constexpr operator==(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  return (a.sign == b.sign) && (a.value == b.value);
}
template <uint64_t before, uint64_t after>
bool constexpr operator!=(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  return !(a == b);
}
template <uint64_t before, uint64_t after>
bool constexpr operator<(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  if (a.sign && !b.sign)
    return true;
  else if (!a.sign && b.sign)
    return false;
  else if (a.sign && b.sign)
    return a > b;
  else
    return a < b;
}
template <uint64_t before, uint64_t after>
bool constexpr operator>(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  if (a.sign && !b.sign)
    return false;
  else if (!a.sign && b.sign)
    return true;
  else if (a.sign && b.sign)
    return a < b;
  else
    return a > b;
}
template <uint64_t before, uint64_t after>
bool constexpr operator<=(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  if (a.sign && !b.sign)
    return true;
  else if (!a.sign && b.sign)
    return false;
  else if (a.sign && b.sign)
    return a >= b;
  else
    return a <= b;
}
template <uint64_t before, uint64_t after>
bool constexpr operator>=(const Fixed<before, after>& a, const Fixed<before, after>& b)
{
  if (a.sign && !b.sign)
    return false;
  else if (!a.sign && b.sign)
    return true;
  else if (a.sign && b.sign)
    return a <= b;
  else
    return a >= b;
}
} // namespace CppUtil