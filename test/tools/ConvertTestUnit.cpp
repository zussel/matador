#include "ConvertTestUnit.hpp"

#include "tools/convert.hpp"
#include "tools/varchar.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>
#include <typeinfo>
#include <iostream>
#include <string>
#include <cerrno>

using namespace oos;

using std::cout;
using std::string;

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

template < class T >
void
convert(const T &from, char *to, size_t num,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
   cout << "SUCCEEDED: floating point > char* (" << typeid(T).name() << ")\n";
   convert(from, to, num, 2);
}

template < class T >
void
convert(const T &from, std::string &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  cout << "DELEGATED: floating point > string (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256);
  to.assign(buf);
}

template < class T >
void
convert(const T &from, oos::varchar_base &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  // cout << "DELEGATED: floating point > varchar (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256);
  to.assign(buf);
}

ConvertTestUnit::ConvertTestUnit()
  : unit_test("convert test unit")
{
  add_test("to_char", std::tr1::bind(&ConvertTestUnit::convert_to_char, this), "convert to char test");
  add_test("to_bool", std::tr1::bind(&ConvertTestUnit::convert_to_bool, this), "convert to bool test");
  add_test("to_short", std::tr1::bind(&ConvertTestUnit::convert_to_short, this), "convert to short test");
  add_test("to_int", std::tr1::bind(&ConvertTestUnit::convert_to_int, this), "convert to int test");
  add_test("to_long", std::tr1::bind(&ConvertTestUnit::convert_to_long, this), "convert to long test");
  add_test("to_unsigned_char", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_char, this), "convert to unsigned char test");
  add_test("to_unsigned_short", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_short, this), "convert to unsigned short test");
  add_test("to_unsigned_int", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_int, this), "convert to unsigned int test");
  add_test("to_unsigned_long", std::tr1::bind(&ConvertTestUnit::convert_to_unsigned_long, this), "convert to unsigned long test");
  add_test("to_float", std::tr1::bind(&ConvertTestUnit::convert_to_float, this), "convert to float test");
  add_test("to_double", std::tr1::bind(&ConvertTestUnit::convert_to_double, this), "convert to double test");
  add_test("to_char_pointer", std::tr1::bind(&ConvertTestUnit::convert_to_char_pointer, this), "convert to const char pointer test");
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

#define CONVERT_EXPECT_SUCCESS_FLOAT(from, to, in, out, precision) \
  try { \
    from a(in); \
    to b; \
    convert(a, b, precision); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }

#define CONVERT_ARRAY_EXPECT_SUCCESS(from, to, in, out) \
  try { \
    from a = in; \
    to b[256]; \
    convert(a, b, 256); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }

#define CONVERT_ARRAY_EXPECT_SUCCESS_FLOAT(from, to, in, out, precision) \
  try { \
    from a = in; \
    to b[256]; \
    convert(a, b, 256, precision); \
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
  CONVERT_EXPECT_SUCCESS(char, bool, 'c', true);
  CONVERT_EXPECT_SUCCESS(bool, bool, true, true);
  CONVERT_EXPECT_SUCCESS(short, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(int, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(long, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(unsigned char, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(unsigned short, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(unsigned int, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(unsigned long, bool, 99, true);
  CONVERT_EXPECT_SUCCESS(float, bool, 99.45f, true);
  CONVERT_EXPECT_SUCCESS(double, bool, 0.0f, false);
  CONVERT_EXPECT_SUCCESS(const char*, bool, "99", true);
  CONVERT_EXPECT_SUCCESS(const char*, bool, "hello", false);
  CONVERT_EXPECT_SUCCESS(std::string, bool, "99", true);
  CONVERT_EXPECT_SUCCESS(std::string, bool, "hello", false);
  // TODO: add varchar converts
  CONVERT_EXPECT_SUCCESS(varchar<8>, bool, "99", true);
  CONVERT_EXPECT_SUCCESS(varchar<8>, bool, "hello", false);
}

void
ConvertTestUnit::convert_to_char()
{
  CONVERT_EXPECT_SUCCESS(char, char, 'c', 'c');
  CONVERT_EXPECT_SUCCESS(bool, char, true, 1);
  CONVERT_EXPECT_FAIL(short, char, 99, 99);
  CONVERT_EXPECT_FAIL(int, char, 99, 99);
  CONVERT_EXPECT_FAIL(long, char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned char, char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned short, char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, char, 99, 99);
  CONVERT_EXPECT_FAIL(float, char, 99, 99);
  CONVERT_EXPECT_FAIL(double, char, 99, 99);
  // TODO: add more tests
  CONVERT_EXPECT_SUCCESS(const char*, char, "99", 99);
  CONVERT_EXPECT_SUCCESS(std::string, char, "99", 99);
  // TODO: add varchar converts
  CONVERT_EXPECT_SUCCESS(varchar<8>, char, "99", 99);
}

void
ConvertTestUnit::convert_to_short()
{
  CONVERT_EXPECT_SUCCESS(char, short, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, short, true, 1);
  CONVERT_EXPECT_SUCCESS(short, short, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, short, 99, 99);
  CONVERT_EXPECT_FAIL(long, short, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned short, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, short, 99, 99);
  CONVERT_EXPECT_FAIL(float, short, 99, 99);
  CONVERT_EXPECT_FAIL(double, short, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, short, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, short, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, short, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, short, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, short, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, short, "99", 99);
}

void
ConvertTestUnit::convert_to_int()
{
  CONVERT_EXPECT_SUCCESS(char, int, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, int, true, 1);
  CONVERT_EXPECT_SUCCESS(short, int, 4711, 4711);
  CONVERT_EXPECT_SUCCESS(int, int, 99, 99);
  if (sizeof(long) <= sizeof(int)) {
    CONVERT_EXPECT_SUCCESS(long, int, 99, 99);
  } else {
    CONVERT_EXPECT_FAIL(long, int, 99, 99);
  }
  // TODO: better conversion; check max and min
  CONVERT_EXPECT_SUCCESS(unsigned char, int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, int, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, int, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, int, 99, 99);
  CONVERT_EXPECT_FAIL(float, int, 99, 99);
  CONVERT_EXPECT_FAIL(double, int, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, int, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, int, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, int, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, int, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, int, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, int, "99", 99);
}

void
ConvertTestUnit::convert_to_long()
{
  CONVERT_EXPECT_SUCCESS(char, long, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, long, true, 1);
  CONVERT_EXPECT_SUCCESS(short, long, 4711, 4711);
  CONVERT_EXPECT_SUCCESS(int, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(long, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, long, 99, 99);
  if (sizeof(long) <= sizeof(unsigned int)) {
    CONVERT_EXPECT_FAIL(unsigned int, long, 99, 99);
  } else {
    CONVERT_EXPECT_SUCCESS(unsigned int, long, 99, 99);
  }
  CONVERT_EXPECT_FAIL(unsigned long, long, 99, 99);
  CONVERT_EXPECT_FAIL(float, long, 99, 99);
  CONVERT_EXPECT_FAIL(double, long, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, long, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, long, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, long, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, long, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, long, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, long, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_char()
{
  CONVERT_EXPECT_FAIL(char, unsigned char, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned char, true, 1);
  CONVERT_EXPECT_FAIL(short, unsigned char, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned char, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned short, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned char, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned char, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned char, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned char, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned char, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, unsigned char, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned char, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, unsigned short, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_short()
{
  CONVERT_EXPECT_FAIL(char, unsigned short, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned short, true, 1);
  CONVERT_EXPECT_FAIL(short, unsigned short, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned short, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, unsigned short, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned int, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned short, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned short, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned short, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned short, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned short, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, unsigned short, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned short, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, unsigned short, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_int()
{
  CONVERT_EXPECT_FAIL(char, unsigned int, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned int, true, 1);
  CONVERT_EXPECT_FAIL(short, unsigned int, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned int, unsigned int, 99, 99);
  if (sizeof(unsigned long) <= sizeof(unsigned int)) {
    CONVERT_EXPECT_SUCCESS(unsigned long, unsigned int, 99, 99);
  } else {
    CONVERT_EXPECT_FAIL(unsigned long, unsigned int, 99, 99);
  }
  CONVERT_EXPECT_FAIL(float, unsigned int, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned int, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned int, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned int, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned int, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, unsigned int, "99", 99);
  CONVERT_EXPECT_SUCCESS(std::string, unsigned int, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned int, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, unsigned int, "99", 99);
}

void
ConvertTestUnit::convert_to_unsigned_long()
{
  CONVERT_EXPECT_FAIL(char, unsigned long, 'c', 99);
  CONVERT_EXPECT_SUCCESS(bool, unsigned long, true, 1);
  CONVERT_EXPECT_FAIL(short, unsigned long, 4711, 4711);
  CONVERT_EXPECT_FAIL(int, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(long, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned char, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned short, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned int, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(unsigned long, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(float, unsigned long, 99, 99);
  CONVERT_EXPECT_FAIL(double, unsigned long, 99, 99);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned long, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, unsigned long, "99", 99);
  CONVERT_EXPECT_FAIL(const char*, unsigned long, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, unsigned long, "99", 99);
  CONVERT_EXPECT_FAIL(std::string, unsigned long, "fail", 99);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, unsigned long, "99", 99);
}

void
ConvertTestUnit::convert_to_float()
{
  CONVERT_EXPECT_FAIL(char, float, 'c', 99.0);
  CONVERT_EXPECT_SUCCESS(bool, float, true, 1.0);
  CONVERT_EXPECT_FAIL(short, float, 4711, 4711.0);
  CONVERT_EXPECT_FAIL(int, float, 99, 99);
  CONVERT_EXPECT_FAIL(long, float, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned char, float, 99, 99.0);
  CONVERT_EXPECT_FAIL(unsigned short, float, 99, 99.0);
  CONVERT_EXPECT_FAIL(unsigned int, float, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, float, 99, 99);
  CONVERT_EXPECT_SUCCESS(float, float, 99.45f, 99.45f);
  CONVERT_EXPECT_FAIL(double, float, 99.45f, 99.45f);
  CONVERT_EXPECT_SUCCESS(const char*, float, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, float, "99.45", 99.45f);
  CONVERT_EXPECT_FAIL(const char*, float, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, float, "99.45", 99.45f);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, float, "99.45", 99.45f);
}

void
ConvertTestUnit::convert_to_double()
{
  // TODO: better converting to double
  CONVERT_EXPECT_FAIL(char, double, 'c', 99.0);
  CONVERT_EXPECT_SUCCESS(bool, double, true, 1.0);
  CONVERT_EXPECT_FAIL(short, double, 4711, 4711.0);
  CONVERT_EXPECT_FAIL(int, double, 99, 99);
  CONVERT_EXPECT_FAIL(long, double, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned char, double, 99, 99.0);
  CONVERT_EXPECT_FAIL(unsigned short, double, 99, 99.0);
  CONVERT_EXPECT_FAIL(unsigned int, double, 99, 99);
  CONVERT_EXPECT_FAIL(unsigned long, double, 99, 99);  
  CONVERT_EXPECT_SUCCESS(float, double, 99.45f, 99.45f);
  CONVERT_EXPECT_SUCCESS(double, double, 99.45, 99.45);
  CONVERT_EXPECT_SUCCESS(const char*, double, "0", 0);
  CONVERT_EXPECT_SUCCESS(const char*, double, "99.45", 99.45);
  CONVERT_EXPECT_FAIL(const char*, double, "fail", 99);
  CONVERT_EXPECT_SUCCESS(std::string, double, "99.45", 99.45);
  // TODO: better conversion; adapt varchar
  CONVERT_EXPECT_SUCCESS(varchar<8>, double, "99.45", 99.45);
}

void
ConvertTestUnit::convert_to_char_pointer()
{
  CONVERT_ARRAY_EXPECT_SUCCESS(char, char, 'c', "c");
  // TODO: check on 1,true,on
  CONVERT_ARRAY_EXPECT_SUCCESS(bool, char, true, "1");
//  CONVERT_ARRAY_EXPECT_SUCCESS(bool, char, true, "true");
  CONVERT_ARRAY_EXPECT_SUCCESS(short, char, -4711, "-4711");
  CONVERT_ARRAY_EXPECT_SUCCESS(int, char, -99, "-99");
  CONVERT_ARRAY_EXPECT_SUCCESS(long, char, -99, "-99");
  CONVERT_ARRAY_EXPECT_SUCCESS(unsigned short, char, 99, "99");
  CONVERT_ARRAY_EXPECT_SUCCESS(unsigned int, char, 99, "99");
  CONVERT_ARRAY_EXPECT_SUCCESS(unsigned long, char, 99, "99");
  CONVERT_ARRAY_EXPECT_SUCCESS_FLOAT(float, char, -99.34f, "-99.34", 2);
  CONVERT_ARRAY_EXPECT_SUCCESS_FLOAT(double, char, -99.34f, "-99.34", 2);
  CONVERT_ARRAY_EXPECT_SUCCESS(double, const char*, -99.34f, "-99.34");
  CONVERT_ARRAY_EXPECT_SUCCESS(const char*, char, "99", "99");
  CONVERT_ARRAY_EXPECT_SUCCESS(std::string, char, "99", "99");
  // TODO: add varchar converts
  CONVERT_ARRAY_EXPECT_SUCCESS(varchar<8>, char, "99", "99");
}

void
ConvertTestUnit::convert_to_string()
{
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
}

void
ConvertTestUnit::convert_to_varchar()
{
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
}
