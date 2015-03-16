/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ConvertTestUnit.hpp"

#include "tools/convert.hpp"
#include "tools/varchar.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>
#include <limits>
#include <typeinfo>
#include <iostream>
#include <string>
#include <cerrno>

using namespace oos;

using std::cout;
using std::string;


ConvertTestUnit::ConvertTestUnit()
  : unit_test("convert", "convert test unit")
{
  add_test("to_char", std::bind(&ConvertTestUnit::convert_to_char, this), "convert to char test");
  add_test("to_bool", std::bind(&ConvertTestUnit::convert_to_bool, this), "convert to bool test");
  add_test("to_short", std::bind(&ConvertTestUnit::convert_to_short, this), "convert to short test");
  add_test("to_int", std::bind(&ConvertTestUnit::convert_to_int, this), "convert to int test");
  add_test("to_long", std::bind(&ConvertTestUnit::convert_to_long, this), "convert to long test");
  add_test("to_unsigned_char", std::bind(&ConvertTestUnit::convert_to_unsigned_char, this), "convert to unsigned char test");
  add_test("to_unsigned_short", std::bind(&ConvertTestUnit::convert_to_unsigned_short, this), "convert to unsigned short test");
  add_test("to_unsigned_int", std::bind(&ConvertTestUnit::convert_to_unsigned_int, this), "convert to unsigned int test");
  add_test("to_unsigned_long", std::bind(&ConvertTestUnit::convert_to_unsigned_long, this), "convert to unsigned long test");
  add_test("to_float", std::bind(&ConvertTestUnit::convert_to_float, this), "convert to float test");
  add_test("to_double", std::bind(&ConvertTestUnit::convert_to_double, this), "convert to double test");
  add_test("to_char_pointer", std::bind(&ConvertTestUnit::convert_to_char_pointer, this), "convert to const char pointer test");
  add_test("to_string", std::bind(&ConvertTestUnit::convert_to_string, this), "convert to string test");
  add_test("to_varchar", std::bind(&ConvertTestUnit::convert_to_varchar, this), "convert to varchar test");
  add_test("to_date", std::bind(&ConvertTestUnit::convert_to_date, this), "convert to date test");
}

ConvertTestUnit::~ConvertTestUnit()
{}

#define CONVERT_NUMERIC_EXPECT_SUCCESS(FROM, TO, BOUNDARY) \
  try { \
    FROM from(std::numeric_limits<FROM>::BOUNDARY()); \
    TO to; \
    convert(from, to); \
    UNIT_ASSERT_EQUAL(to, std::numeric_limits<FROM>::BOUNDARY(), "convert failed: "#BOUNDARY" values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#FROM" to "#TO" "#BOUNDARY" values must not fail"); \
  }

#define CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK(FROM, TO, BOUNDARY) \
  try { \
    FROM from(std::numeric_limits<TO>::BOUNDARY()); \
    TO to; \
    convert(from, to); \
    UNIT_ASSERT_EQUAL(to, std::numeric_limits<TO>::BOUNDARY(), "convert failed: "#BOUNDARY" values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#FROM" to "#TO" "#BOUNDARY" values must not fail"); \
  }

#define CONVERT_EXPECT_SUCCESS(from, in, to, out) \
  try { \
    from a(in); \
    to b; \
    convert(a, b); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#from" to "#to" must not fail"); \
  }

#define CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE(from, in, to, out, size) \
  try { \
    from a(in); \
    to b[size]; \
    convert(a, b, size); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#from" to "#to" must not fail"); \
  }

#define CONVERT_EXPECT_SUCCESS_SIZE(from, in, to, out, size) \
  try { \
    from a(in); \
    to b; \
    convert(a, b, size); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#from" to "#to" must not fail"); \
  }

#define CONVERT_EXPECT_SUCCESS_SIZE_PRECISION(from, in, to, out, size, precision) \
  try { \
    from a(in); \
    to b[size]; \
    convert(a, b, size, precision); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convertion from "#from" to "#to" must not fail"); \
  }

#define CONVERT_NUMERIC_EXPECT_FAILURE(FROM, TO, BOUNDARY) \
  { \
    FROM from(std::numeric_limits<FROM>::BOUNDARY()); \
    TO to; \
    const char *msg = "convertion from "#FROM" to "#TO" "#BOUNDARY" values must fail"; \
    UNIT_ASSERT_EXCEPTION(convert(from, to), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_NUMERIC_EXPECT_FAILURE_SIZE(FROM, TO, BOUNDARY, SIZE) \
  { \
    FROM from(std::numeric_limits<FROM>::BOUNDARY()); \
    TO to; \
    const char *msg = "convertion from "#FROM" to "#TO" "#BOUNDARY" values must fail"; \
    UNIT_ASSERT_EXCEPTION(convert(from, to, SIZE), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(FROM, TO, BOUNDARY, SIZE, PRECISION) \
  { \
    FROM from(std::numeric_limits<FROM>::BOUNDARY()); \
    TO to; \
    const char *msg = "convertion from "#FROM" to "#TO" "#BOUNDARY" values must fail"; \
    UNIT_ASSERT_EXCEPTION(convert(from, to, SIZE, PRECISION), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_EXPECT_FAILURE(FROM, IN, TO, OUT) \
  { \
    FROM from(IN); \
    TO to; \
    const char *msg = "convertion from "#FROM" to "#TO" must fail"; \
    UNIT_ASSERT_EXCEPTION(convert(from, to), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_TO_PTR_EXPECT_FAILURE(FROM, IN, TO, OUT) \
  { \
    FROM from(IN); \
    TO to(0); \
    const char *msg("convertion from "#FROM" to "#TO" must fail"); \
    UNIT_ASSERT_EXCEPTION(convert(from, to), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_EXPECT_FAILURE_SIZE(FROM, IN, TO, OUT, SIZE) \
  { \
    FROM from(IN); \
    TO to; \
    const char *msg("convertion from "#FROM" to "#TO" must fail"); \
    UNIT_ASSERT_EXCEPTION(convert(from, to, SIZE), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

#define CONVERT_EXPECT_FAILURE_SIZE_PRECISION(FROM, IN, TO, OUT, SIZE, PRECISION) \
  { \
    FROM from(IN); \
    TO to; \
    const char *msg("convertion from "#FROM" to "#TO" must fail"); \
    UNIT_ASSERT_EXCEPTION(convert(from, to, SIZE, PRECISION), std::bad_cast, "std::bad_cast", msg) \
  } while(false);

void
ConvertTestUnit::convert_to_bool()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (char, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (char, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (short, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (short, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (int, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (int, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (long, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (long, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned char, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned char, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned short, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned short, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned int, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned int, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned long, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (unsigned long, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (float, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (float, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, bool, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (double, bool, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS_WEAK          (double, bool, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, bool, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, bool, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", bool, true);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", bool, false);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", bool, true, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", bool, true, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", bool, true);
  CONVERT_EXPECT_FAILURE               (string, "hello", bool, false);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", bool, true, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", bool, true, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", bool, true);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", bool, false);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", bool, true, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", bool, true, 256, 3);

  CONVERT_EXPECT_FAILURE               (oos::date, 2457090, bool, false);
  CONVERT_EXPECT_FAILURE_SIZE          (oos::date, 2457090, bool, true, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(oos::date, 2457090, bool, true, 256, 3);
}

void
ConvertTestUnit::convert_to_char()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, char, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, char, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (short, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (short, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (int, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (long, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned char, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned char, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, char, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", char, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", char, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", char, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", char, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", char, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", char, 99, 256, 3);

  CONVERT_EXPECT_FAILURE               (oos::date, 2457090, char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (oos::date, 2457090, char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(oos::date, 2457090, char, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_short()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (int, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (long, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, short, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", short, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", short, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", short, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", short, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", short, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", short, 99, 256, 3);

  CONVERT_EXPECT_FAILURE               (oos::date, 2457090, short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (oos::date, 2457090, short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(oos::date, 2457090, short, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_int()
{
  /*
  cout << "\nstd::numeric_limits<int>::digits: " << std::numeric_limits<int>::digits;
  cout << "\n(int)std::numeric_limits<float>::max(): " << (int)std::numeric_limits<float>::max() << "\n";
  cout << "\nstd::numeric_limits<float>::max(): " << std::numeric_limits<float>::max() << "\n";
  cout << "\nstd::numeric_limits<int>::min(): " << std::numeric_limits<int>::min() << "\n";
  cout << "\nstd::numeric_limits<int>::max(): " << std::numeric_limits<int>::max() << "\n";
  cout << "\nstd::numeric_limits<float>::digits: " << std::numeric_limits<float>::digits << "\n";
  cout << "\nstd::numeric_limits<long>::digits: " << std::numeric_limits<long>::digits;
  cout << "\nstd::numeric_limits<double>::digits: " << std::numeric_limits<double>::digits << "\n";
  cout << "\nstd::numeric_limits<unsigned long>::digits: " << std::numeric_limits<unsigned long>::digits << "\n";
  */

  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (int, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (int, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, int, min, 256, 3);

  if (sizeof(long) <= sizeof(int)) {
    CONVERT_NUMERIC_EXPECT_SUCCESS             (long, int, min);
    CONVERT_NUMERIC_EXPECT_SUCCESS             (long, int, max);
  } else {
    CONVERT_NUMERIC_EXPECT_FAILURE             (long, int, min);
    CONVERT_NUMERIC_EXPECT_FAILURE             (long, int, max);
  }
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, int, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", int, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", int, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", int, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", int, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", int, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", int, 99, 256, 3);

  CONVERT_EXPECT_FAILURE               (oos::date, 2457090, int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (oos::date, 2457090, int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(oos::date, 2457090, int, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_long()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (int, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (int, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (long, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (long, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, long, min, 256, 3);

  if (sizeof(unsigned int) < sizeof(long)) {
    CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, long, min);
    CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, long, max);
  } else {
    CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, long, min);
    CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, long, max);
  }
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, long, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, long, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, long, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, long, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", long, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", long, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", long, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", long, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", long, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", long, 99, 256, 3);

  CONVERT_EXPECT_FAILURE               (oos::date, 2457090, long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (oos::date, 2457090, long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(oos::date, 2457090, long, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_unsigned_char()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (char, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (short, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (short, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned short, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, unsigned char, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned char, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned char, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, unsigned char, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, unsigned char, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", unsigned char, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", unsigned char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", unsigned char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", unsigned char, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", unsigned char, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", unsigned char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", unsigned char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", unsigned char, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", unsigned char, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", unsigned char, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", unsigned char, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", unsigned char, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_unsigned_short()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (char, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (short, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned int, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, unsigned short, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned short, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned short, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, unsigned short, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, unsigned short, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", unsigned short, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", unsigned short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", unsigned short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", unsigned short, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", unsigned short, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", unsigned short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", unsigned short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", unsigned short, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", unsigned short, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", unsigned short, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", unsigned short, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", unsigned short, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_unsigned_int()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (char, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (short, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (int, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned int, min);
  if (sizeof(unsigned int) < sizeof(long)) {
    CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned int, max);
  } else {
    CONVERT_NUMERIC_EXPECT_SUCCESS               (long, unsigned int, max);
  }
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, unsigned int, min, 256, 3);

  if (sizeof(unsigned int) < sizeof(unsigned long)) {
    CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned int, min);
    CONVERT_NUMERIC_EXPECT_FAILURE               (unsigned long, unsigned int, max);
  } else {
    CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned long, unsigned int, min);
    CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned long, unsigned int, max);
  }
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, unsigned int, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned int, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned int, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, unsigned int, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, unsigned int, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", unsigned int, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", unsigned int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", unsigned int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", unsigned int, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", unsigned int, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", unsigned int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", unsigned int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", unsigned int, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", unsigned int, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", unsigned int, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", unsigned int, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", unsigned int, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_unsigned_long()
{
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (bool, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (bool, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(bool, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (char, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (char, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (char, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(char, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (short, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (short, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (short, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(short, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (int, unsigned long, min);
  if (sizeof(int) < sizeof(long)) {
    CONVERT_NUMERIC_EXPECT_SUCCESS               (int, unsigned long, max);
  } else {
    CONVERT_NUMERIC_EXPECT_SUCCESS               (int, unsigned long, max);
  }
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (int, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(int, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (long, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (long, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (long, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(long, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned char, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned char, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned short, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned short, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned int, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned int, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned long, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_SUCCESS               (unsigned long, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (unsigned long, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (float, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (float, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(float, unsigned long, min, 256, 3);

  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned long, min);
  CONVERT_NUMERIC_EXPECT_FAILURE               (double, unsigned long, max);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE          (double, unsigned long, min, 256);
  CONVERT_NUMERIC_EXPECT_FAILURE_SIZE_PRECISION(double, unsigned long, min, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99", unsigned long, 99);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", unsigned long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", unsigned long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", unsigned long, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99", unsigned long, 99);
  CONVERT_EXPECT_FAILURE               (string, "hello", unsigned long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", unsigned long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", unsigned long, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", unsigned long, 99);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", unsigned long, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", unsigned long, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", unsigned long, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_float()
{
  CONVERT_EXPECT_SUCCESS               (bool, true, float, 1.0);
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, float, 1.0, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, float, 1.0, 256, 3);

  CONVERT_EXPECT_SUCCESS               (char, -99, float, -99);
  CONVERT_EXPECT_SUCCESS               (char, 'c', float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (short, -99, float, -99);
  CONVERT_EXPECT_SUCCESS               (short, 99, float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, float, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, -99, float, -99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (int, -99, float, -99);
  CONVERT_EXPECT_SUCCESS               (int, 99, float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, float, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, -99, float, -99, 256, 3);

  if (sizeof(long) > sizeof(float)) {
    CONVERT_EXPECT_FAILURE               (long, -99, float, -99);
    CONVERT_EXPECT_FAILURE               (long, 99, float, 99);
  } else {
    CONVERT_EXPECT_SUCCESS               (long, -99, float, -99);
    CONVERT_EXPECT_SUCCESS               (long, 99, float, 99);
  }
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, float, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, -99, float, -99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned char, 'c', float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned short, 99, float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned int, 99, float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, float, 99, 256, 3);

  if (sizeof(unsigned long) > sizeof(float)) {
    CONVERT_EXPECT_FAILURE               (unsigned long, 99, float, 99);
  } else {
    CONVERT_EXPECT_SUCCESS               (unsigned long, 99, float, 99);
  }
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (float, 99.34567f, float, 99.34567f);
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.34567f, float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.34567f, float, 99, 256, 3);

  CONVERT_EXPECT_FAILURE               (double, 99.34567, float, 99.34567f);
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.34567, float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.34567, float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99.456", float, 99.456f);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99.456", float, 99.456f);
  CONVERT_EXPECT_FAILURE               (string, "hello", float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", float, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99.456", float, 99.456f);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", float, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", float, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", float, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_double()
{
  CONVERT_EXPECT_SUCCESS               (bool, true, double, 1.0);
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, double, 1.0, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, double, 1.0, 256, 3);

  CONVERT_EXPECT_SUCCESS               (char, -99, double, -99);
  CONVERT_EXPECT_SUCCESS               (char, 'c', double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (short, -99, double, -99);
  CONVERT_EXPECT_SUCCESS               (short, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, double, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, -99, double, -99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (int, -99, double, -99);
  CONVERT_EXPECT_SUCCESS               (int, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, double, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, -99, double, -99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (long, -99, double, -99);
  CONVERT_EXPECT_SUCCESS               (long, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, double, -99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, -99, double, -99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned char, 'c', double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned short, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned int, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (unsigned long, 99, double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (float, 99.34567f, double, 99.34567f);
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.34567f, double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.34567f, double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (double, 99.34567, double, 99.34567);
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.34567, double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.34567, double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "99.456", double, 99.456);
  CONVERT_EXPECT_FAILURE               (const char*, "hello", double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (string, "99.456", double, 99.456);
  CONVERT_EXPECT_FAILURE               (string, "hello", double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", double, 99, 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99.456", double, 99.456);
  CONVERT_EXPECT_FAILURE               (varchar<16>, "hello", double, 99);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", double, 99, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", double, 99, 256, 3);
}

void
ConvertTestUnit::convert_to_char_pointer()
{
  CONVERT_TO_PTR_EXPECT_FAILURE        (bool, true, char*, "true");
  CONVERT_TO_PTR_EXPECT_FAILURE        (bool, false, char*, "false");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (bool, true, char, "true", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, char, "true", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (char, 'c', char*, "c");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (char, 'c', char, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', char, "c", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (short, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (short, 99, char, "99", 256);
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (short, -99, char, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (int, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (int, 99, char, "99", 256);
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (int, -99, char, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (long, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (long, 99, char, "99", 256);
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (long, -99, char, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (unsigned char, 'c', char*, "c");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (unsigned char, 'c', char, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', char, "c", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (unsigned short, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (unsigned short, 99, char, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (unsigned int, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (unsigned int, 99, char, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (unsigned long, 99, char*, "99");
  CONVERT_TO_ARRAY_EXPECT_SUCCESS_SIZE (unsigned long, 99, char, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, char, "99", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (float, 99.4567f, char*, "99.457");
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.4567f, char, "99.457", 256);
  CONVERT_EXPECT_SUCCESS_SIZE_PRECISION(float, 99.4567f, char, "99.457", 256, 3);

  CONVERT_TO_PTR_EXPECT_FAILURE        (double, 99.4567, char*, "99.457");
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.4567, char, "99.457", 256);
  CONVERT_EXPECT_SUCCESS_SIZE_PRECISION(double, 99.4567, char, "99.457", 256, 3);
}

void
ConvertTestUnit::convert_to_string()
{
  CONVERT_EXPECT_SUCCESS               (bool, true, std::string, "true");
  CONVERT_EXPECT_SUCCESS               (bool, false, std::string, "false");
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, std::string, "true", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, std::string, "true", 256, 3);

  CONVERT_EXPECT_SUCCESS               (char, 'c', std::string, "c");
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', std::string, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', std::string, "c", 256, 3);

  CONVERT_EXPECT_SUCCESS        (short, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (short, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, std::string, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (int, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (int, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, std::string, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (long, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (long, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, std::string, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned char, 'c', std::string, "c");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', std::string, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', std::string, "c", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned short, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned int, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned long, 99, std::string, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, std::string, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, std::string, "99", 256, 3);

  CONVERT_EXPECT_FAILURE               (float, 99.4567f, std::string, "99.457");
  CONVERT_EXPECT_SUCCESS_SIZE          (float, 99.4567f, std::string, "99.457", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.4567f, std::string, "99.457", 256, 3);

  CONVERT_EXPECT_FAILURE               (double, 99.4567, std::string, "99.457");
  CONVERT_EXPECT_SUCCESS_SIZE          (double, 99.4567, std::string, "99.457", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.4567, std::string, "99.457", 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "varchar", std::string, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "varchar", std::string, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "varchar", std::string, "varchar", 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<64>, "varchar", std::string, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<64>, "varchar", std::string, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<64>, "varchar", std::string, "varchar", 256, 3);

  CONVERT_EXPECT_SUCCESS               (std::string, "varchar", std::string, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (std::string, "varchar", std::string, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(std::string, "varchar", std::string, "varchar", 256, 3);

  /*
  CONVERT_EXPECT_SUCCESS(char, std::string, 'c', "c");
  // TODO: check on 1,true,on
//  CONVERT_EXPECT_SUCCESS(bool, std::string, true, "true");
  CONVERT_EXPECT_SUCCESS(bool, std::string, true, "1");
  CONVERT_EXPECT_SUCCESS(short, std::string, -4711, "-4711");
  CONVERT_EXPECT_SUCCESS(int, std::string, -99, "-99");
  CONVERT_EXPECT_SUCCESS(long, std::string, -99, "-99");
  CONVERT_EXPECT_SUCCESS(unsigned short, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned int, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned long, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS_FLOAT(float, std::string, -99.34f, "-99.34", 2);
  CONVERT_EXPECT_SUCCESS_FLOAT(double, std::string, -99.34f, "-99.34", 2);
  CONVERT_EXPECT_SUCCESS(double, std::string, -99.34f, "-99.34");
  CONVERT_EXPECT_SUCCESS(const char*, std::string, "99", "99");
  CONVERT_EXPECT_SUCCESS(std::string, std::string, "99", "99");
  // TODO: add varchar converts
  CONVERT_EXPECT_SUCCESS(varchar<8>, std::string, "99", "99");
  */
}

void
ConvertTestUnit::convert_to_varchar()
{
  CONVERT_EXPECT_SUCCESS               (bool, true, varchar<64>, "true");
  CONVERT_EXPECT_SUCCESS               (bool, false, varchar<64>, "false");
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, varchar<64>, "true", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, varchar<64>, "true", 256, 3);

  CONVERT_EXPECT_SUCCESS        (char, 'c', varchar<64>, "c");
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', varchar<64>, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', varchar<64>, "c", 256, 3);

  CONVERT_EXPECT_SUCCESS        (short, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (short, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, varchar<64>, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (int, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (int, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, varchar<64>, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (long, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (long, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, varchar<64>, "-99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned char, 'c', varchar<64>, "c");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', varchar<64>, "c", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', varchar<64>, "c", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned short, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned int, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_SUCCESS        (unsigned long, 99, varchar<64>, "99");
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, varchar<64>, "99", 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, varchar<64>, "99", 256, 3);

  CONVERT_EXPECT_FAILURE               (float, 99.4567f, varchar<64>, "99.457");
  CONVERT_EXPECT_SUCCESS_SIZE          (float, 99.4567f, varchar<64>, "99.457", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.4567f, varchar<64>, "99.457", 256, 3);

  CONVERT_EXPECT_FAILURE               (double, 99.4567, varchar<64>, "99.457");
  CONVERT_EXPECT_SUCCESS_SIZE          (double, 99.4567, varchar<64>, "99.457", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.4567, varchar<64>, "99.457", 256, 3);

  CONVERT_EXPECT_SUCCESS               (const char*, "varchar", varchar<64>, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "varchar", varchar<64>, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "varchar", varchar<64>, "varchar", 256, 3);

  CONVERT_EXPECT_SUCCESS               (varchar<64>, "varchar", varchar<64>, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<64>, "varchar", varchar<64>, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<64>, "varchar", varchar<64>, "varchar", 256, 3);

  CONVERT_EXPECT_SUCCESS               (std::string, "varchar", varchar<64>, "varchar");
  CONVERT_EXPECT_FAILURE_SIZE          (std::string, "varchar", varchar<64>, "varchar", 3);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(std::string, "varchar", varchar<64>, "varchar", 256, 3);

  /*
  CONVERT_EXPECT_SUCCESS(char, varchar<64>, 'c', "c");
  // TODO: check on 1,true,on
//  CONVERT_EXPECT_SUCCESS(bool, varchar<64>, false, "false");
  CONVERT_EXPECT_SUCCESS(bool, varchar<64>, false, "0");
  CONVERT_EXPECT_SUCCESS(short, varchar<64>, -4711, "-4711");
  CONVERT_EXPECT_SUCCESS(int, varchar<64>, -99, "-99");
  CONVERT_EXPECT_SUCCESS(long, varchar<64>, -99, "-99");
  CONVERT_EXPECT_SUCCESS(unsigned short, varchar<64>, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned int, varchar<64>, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned long, varchar<64>, 99, "99");
  CONVERT_EXPECT_SUCCESS_FLOAT(float, varchar<64>, -99.34f, "-99.34", 2);
  CONVERT_EXPECT_SUCCESS_FLOAT(double, varchar<64>, -99.34f, "-99.34", 2);
  CONVERT_EXPECT_SUCCESS(const char*, varchar<64>, "99", "99");
  CONVERT_EXPECT_SUCCESS(std::string, varchar<64>, "99", "99");
  // TODO: add varchar conversions
  CONVERT_EXPECT_SUCCESS(varchar<8>, varchar<64>, "99", "99");
  */
}

void ConvertTestUnit::convert_to_date()
{
  CONVERT_EXPECT_FAILURE(bool, true, date, 0);
  CONVERT_EXPECT_FAILURE_SIZE(bool, true, date, 0, 256);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, date, 0, 256, 3);

  CONVERT_EXPECT_SUCCESS(int, 2457090, oos::date, oos::date(2457090));
}

void ConvertTestUnit::convert_to_time()
{

}
