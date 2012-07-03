#include "ConvertTestUnit.hpp"

//#include "tools/convert.hpp"
#include "tools/varchar.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>
#include <typeinfo>
#include <iostream>
#include <string>

using namespace oos;

using std::cout;
using std::string;

template < typename T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<!std::tr1::is_same<char, T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_fundamental<T>::value >::type* = 0)
{
  to = from > 0;
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) < sizeof(U)>::type* = 0)
{
  to = from;
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<bool, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) <= sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

template < typename T, typename U >
typename oos::enable_if<!std::tr1::is_convertible<T, U>::value >::type
convert(const T &from, U &to)
{
  throw std::bad_cast();
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0)
{
  throw std::bad_cast();
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::tr1::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_signed<U>::value >::type* = 0)
{
  throw std::bad_cast();
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<char, T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0)
{
  to = static_cast<U>(from);
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<U, T>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<bool, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_convertible<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) == sizeof(U)>::type* = 0)
{
  throw std::bad_cast();
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<U, T>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<bool, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_convertible<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) == sizeof(U)>::type* = 0)
{
  throw std::bad_cast();
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<U, T>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<bool, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) == sizeof(U)>::type* = 0)
{
  to = from;
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<U, T>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<char, U>::value >::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<bool, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) == sizeof(U)>::type* = 0)
{
  to = from;
}

void
convert(const char *from, char &to)
{
  throw std::bad_cast();
}

template < typename T >
void
convert(const T &from, T &to)
{
  to = from;
}

ConvertTestUnit::ConvertTestUnit()
  : unit_test("convert test unit")
{
  add_test("to_char", std::tr1::bind(&ConvertTestUnit::convert_to_char, this), "convert to char test");
  add_test("to_bool", std::tr1::bind(&ConvertTestUnit::convert_to_bool, this), "convert to bool test");
  add_test("to_short", std::tr1::bind(&ConvertTestUnit::convert_to_short, this), "convert to short test");
  add_test("to_int", std::tr1::bind(&ConvertTestUnit::convert_to_int, this), "convert to int test");
  add_test("to_long", std::tr1::bind(&ConvertTestUnit::convert_to_long, this), "convert to long test");
  add_test("to_unsigned_short", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_short, this), "convert to unsigned short test");
  add_test("to_unsigned_int", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_int, this), "convert to unsigned int test");
  add_test("to_unsigned_long", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_long, this), "convert to unsigned long test");
  add_test("to_const_char_pointer", std::tr1::bind(&ConvertTestUnit::convert_to_const_char_pointer, this), "convert to const char pointer test");
  add_test("to_string", std::tr1::bind(&ConvertTestUnit::convert_to_string, this), "convert to string test");
  add_test("to_varchar", std::tr1::bind(&ConvertTestUnit::convert_to_varchar, this), "convert to varchar test");
}

ConvertTestUnit::~ConvertTestUnit()
{}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CONVERT_EXPECT_SUCCESS(from, to, in, out) \
  try { \
    from a(in); \
    to b; \
    convert(a, b); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }

#define CONVERT_EXPECT_FAIL(from, to, in, out) \
  try { \
    from a(in); \
    to b; \
    convert(a, b); \
    UNIT_FAIL("convert must not fail"); \
  } catch (std::bad_cast &) { \
  }

void
ConvertTestUnit::convert_to_bool()
{
  UNIT_FAIL("not implemented");
}

void
ConvertTestUnit::convert_to_char()
{
  CONVERT_EXPECT_SUCCESS(char, char, 'c', 'c');
  CONVERT_EXPECT_SUCCESS(bool, char, true, 1);
  CONVERT_EXPECT_SUCCESS(short, char, 99, 99);
  CONVERT_EXPECT_SUCCESS(int, char, 99, 99);
  CONVERT_EXPECT_SUCCESS(long, char, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, char, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned int, char, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned long, char, 99, 99);
  CONVERT_EXPECT_FAIL(float, char, 99, 99);
  CONVERT_EXPECT_FAIL(double, char, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, char, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, char, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, char, "99", 99);
}

void
ConvertTestUnit::convert_to_short()
{
  CONVERT_EXPECT_SUCCESS(char, short, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, short, true, 1);
  CONVERT_EXPECT_SUCCESS(short, short, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, short, 99, 99);
  CONVERT_EXPECT_FAIL(long, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned short, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, short, 99, 99);
  CONVERT_EXPECT_FAIL(float, short, 99, 99);
  CONVERT_EXPECT_FAIL(double, short, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, short, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, short, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, short, "99", 99);
}

void
ConvertTestUnit::convert_to_int()
{
  CONVERT_EXPECT_SUCCESS(char, int, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, int, true, 1);
  CONVERT_EXPECT_SUCCESS(short, int, 4711, 4711);
  CONVERT_EXPECT_SUCCESS(int, int, 99, 99);
  CONVERT_EXPECT_SUCCESS(long, int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, int, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, int, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, int, 99, 99);
  CONVERT_EXPECT_FAIL(float, int, 99, 99);
  CONVERT_EXPECT_FAIL(double, int, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, int, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, int, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, int, "99", 99);
}

void
ConvertTestUnit::convert_to_long()
{
  CONVERT_EXPECT_SUCCESS(char, long, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, long, true, 1);
  CONVERT_EXPECT_SUCCESS(short, long, 4711, 4711);
  CONVERT_EXPECT_SUCCESS(int, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(long, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, long, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, long, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, long, 99, 99);
  CONVERT_EXPECT_FAIL(float, long, 99, 99);
  CONVERT_EXPECT_FAIL(double, long, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, long, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, long, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, long, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_short()
{
  CONVERT_EXPECT_SUCCESS(char, unsigned short, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned short, true, 1);
  CONVERT_EXPECT_FAIL(short, unsigned short, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned short, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned short, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned short, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, unsigned short, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_int()
{
  CONVERT_EXPECT_SUCCESS(char, unsigned int, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned int, true, 1);
  CONVERT_EXPECT_SUCCESS(short, unsigned int, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned int, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned long, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned int, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned int, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, unsigned int, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_long()
{
  CONVERT_EXPECT_SUCCESS(char, unsigned long, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned long, true, 1);
  CONVERT_EXPECT_SUCCESS(short, unsigned long, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned int, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned long, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned long, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned long, "99", 99);
  CONVERT_EXPECT_FAIL(varchar<8>, unsigned long, "99", 99);
}

void
ConvertTestUnit::convert_to_const_char_pointer()
{
  UNIT_FAIL("not implemented");
}

void
ConvertTestUnit::convert_to_string()
{
  /*
  try {
    char a('c');
    std::string b("c");
    convert(a, b);
    UNIT_ASSERT_EQUAL(b, "c", "convert failed: values are not equal");
  } catch (std::bad_cast &) {
    UNIT_FAIL("convert must not fail");
  }
  */
  CONVERT_EXPECT_SUCCESS(char, std::string, 'c', "c");
  CONVERT_EXPECT_SUCCESS(bool, std::string, 1, "1");
  CONVERT_EXPECT_SUCCESS(short, std::string, -4711, "-4711");
  CONVERT_EXPECT_SUCCESS(int, std::string, -99, "-99");
  CONVERT_EXPECT_SUCCESS(long, std::string, -99, "-99");
  CONVERT_EXPECT_SUCCESS(unsigned short, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned int, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS(unsigned long, std::string, 99, "99");
  CONVERT_EXPECT_SUCCESS(float, std::string, -99.34f, "99.34");
  CONVERT_EXPECT_SUCCESS(double, std::string, -99.34f, "99.34");
//  CONVERT_EXPECT_SUCCESS(const char*, std::string, "99", "99");
  CONVERT_EXPECT_SUCCESS(std::string, std::string, "99", "99");
  CONVERT_EXPECT_SUCCESS(varchar<8>, std::string, "99", "99");
}

void
ConvertTestUnit::convert_to_varchar()
{
  UNIT_FAIL("not implemented");
}
