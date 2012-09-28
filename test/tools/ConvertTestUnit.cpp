#include "ConvertTestUnit.hpp"

//#include "tools/convert.hpp"
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

typedef enum {
  convert_strict = 1,
  convert_fitting = 2,
  convert_weak = 3
} t_convert_policy;

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is smaller than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_fitting || CP == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_signed<T>::value &&
                                 std::tr1::is_signed<U>::value) ||
                                (std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_unsigned<U>::value) ||
                                (std::tr1::is_signed<T>::value &&
                                 std::tr1::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to integral (fitting/weak,  less, " << from << " -> " << to << ")\n";
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_signed<T>::value &&
                                 std::tr1::is_signed<U>::value) ||
                                (std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_unsigned<U>::value) ||
                                (std::tr1::is_signed<T>::value &&
                                 std::tr1::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to integral (strict,        less, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_fitting || CP == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_signed<T>::value &&
                                 std::tr1::is_signed<U>::value) ||
                                (std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_unsigned<U>::value) ||
                                (std::tr1::is_signed<T>::value &&
                                 std::tr1::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to integral (fitting/weak, greater equal, " << from << " -> " << to << ")\n";
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_signed<T>::value &&
                                 std::tr1::is_signed<U>::value) ||
                                (std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_unsigned<U>::value) ||
                                (std::tr1::is_signed<T>::value &&
                                 std::tr1::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to integral (strict,       greater equal, " << from << " -> " << to << ")\n";
}

/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   less
 */
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_fitting || P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "failed integral less unsigned fitting\n";
}

template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral less unsigned weak\n";
  to = from;
}

/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   greater equal
 */
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_fitting || P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "failed integral greater unsigned fitting\n";
}

template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::tr1::is_unsigned<T>::value &&
                                 std::tr1::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral greater unsigned weak\n";
  to = from;
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to floating (fitting/weak,  less, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to floating (strict,        less, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Cannot convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to floating (weak,         greater, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_fitting || P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to floating (fitting/strict,  greater, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to floating (fitting/weak,  less, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to floating (strict,        less, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to floating (fitting/weak,  greater, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value &&
                                std::tr1::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "integral to floating (strict,        greater, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    integral U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to integral (weak/fitting,  less, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to integral (strict,        less, " << from << " -> " << to << ")\n";
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    integral U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to integral (weak/fitting,  greater, " << from << " -> " << to << ")\n";
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value &&
                                std::tr1::is_integral<U>::value &&
                                !std::tr1::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  cout << "floating to integral (strict,        greater, " << from << " -> " << to << ")\n";
}

/*****************
 * from
 *   bool
 * to
 *   arithmetic
 *
 *****************/
template < int P, class T >
void
convert(bool from, T &to,
        typename oos::enable_if<P == convert_fitting || P == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_arithmetic<T>::value &&
                                !std::tr1::is_same<T, bool>::value>::type* = 0)
{
  to = (from ? 1 : 0);
}

template < int P, class T >
void
convert(bool from, T &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_arithmetic<T>::value &&
                                !std::tr1::is_same<T, bool>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*****************
 * from
 *   arithmetic
 * to
 *   bool
 *
 *****************/
template < int P, class T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<P == convert_fitting || P == convert_weak>::type* = 0,
        typename oos::enable_if<std::tr1::is_arithmetic<T>::value &&
                                !std::tr1::is_same<T, bool>::value>::type* = 0)
{
  to = from > 0;
}

template < int P, class T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<P == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_arithmetic<T>::value &&
                                !std::tr1::is_same<T, bool>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*****************
 * from
 *   bool
 * to
 *   bool
 *
 *****************/
template < int P >
void
convert(bool &from, bool &to,
        typename oos::enable_if<P == convert_strict || P == convert_fitting || P == convert_weak>::type* = 0)
{
  to = from;
}

/*
 * from
 *   floating point
 * to
 *   char array
 */
template < int CP, class T, class S, class P >
void
convert(const T &from, char *to, S size, P precision,
        typename oos::enable_if<CP == convert_weak ||
                                CP == convert_fitting ||
                                CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<P>::value>::type* = 0)
{
  cout << "convert (T,char*,S,P) fallback (with CP: " << CP << " T: " << typeid(T).name() << ", size: " << size << ", precision: "<< precision << ")\n";
}

/*
 * from
 *   T
 * to
 *   char array
 */
template < int CP, class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<CP == convert_weak ||
                                CP == convert_fitting ||
                                CP == convert_strict>::type* = 0,
        typename oos::enable_if<!std::tr1::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0)
{
  cout << "convert (T,char*,S) fallback (with CP: " << CP << " T: " << typeid(T).name() << ", size: " << size << ")\n";
}

/*
 * from
 *   floating point
 * to
 *   string
 */
template < int CP, class T, class P >
void
convert(const T &from, std::string &to, P precision,
        typename oos::enable_if<CP == convert_weak ||
                                CP == convert_fitting ||
                                CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<P>::value>::type* = 0)
{
  cout << "convert (T,string,P) fallback (with CP: " << CP << " T: " << typeid(T).name() << ", precision: " << precision << ")\n";
}

template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<(CP == convert_weak ||
                                 CP == convert_fitting ||
                                 CP == convert_strict)>::type* = 0,
        typename oos::enable_if<(!std::tr1::is_arithmetic<T>::value ||
                                 !std::tr1::is_arithmetic<U>::value)>::type* = 0,
        typename oos::enable_if<!std::tr1::is_same<T, U>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP, class T, class U, class S >
void
convert(const T &from, U &to, S size,
        typename oos::enable_if<CP == convert_weak ||
                                CP == convert_fitting ||
                                CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP, class T, class U, class S, class P >
void
convert(const T &from, U &to, S size, P precision,
        typename oos::enable_if<CP == convert_weak ||
                                CP == convert_fitting ||
                                CP == convert_strict>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<P>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < class T, class U >
void
convert(const T &from, U &to)
{
  convert<convert_fitting>(from, to);
}

template < class T, class U, class S >
void
convert(const T &from, U &to, S size,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0)
{
  convert<convert_fitting>(from, to, size);
}

template < class T, class U, class S, class P >
void
convert(const T &from, U &to, S size, P precision,
        typename oos::enable_if<std::tr1::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<P>::value>::type* = 0)
{
  convert<convert_fitting>(from, to, size, precision);
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

#define CONVERT_EXPECT_SUCCESS(from, in, to, out, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }

#define CONVERT_EXPECT_SUCCESS_SIZE(from, in, to, out, size, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b, size); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }

#define CONVERT_EXPECT_SUCCESS_SIZE_PRECISION(from, in, to, out, size, precision, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b, size, precision); \
    UNIT_ASSERT_EQUAL(b, out, "convert failed: values are not equal"); \
  } catch (std::bad_cast &) { \
    UNIT_FAIL("convert must not fail"); \
  }
/*
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
*/
#define CONVERT_EXPECT_FAILURE(from, in, to, out, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b); \
    UNIT_FAIL("convertion "#from" to "#to" must not fail"); \
  } catch (std::bad_cast &) { \
  }

#define CONVERT_EXPECT_FAILURE_SIZE(from, in, to, out, size, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b, size); \
    UNIT_FAIL("convertion "#from" to "#to" must not fail"); \
  } catch (std::bad_cast &) { \
  }

#define CONVERT_EXPECT_FAILURE_SIZE_PRECISION(from, in, to, out, size, precision, policy) \
  try { \
    from a(in); \
    to b; \
    convert<policy>(a, b, size, precision); \
    UNIT_FAIL("convert must not fail"); \
  } catch (std::bad_cast &) { \
  }
/*
#define CONVERT_WITH_SIZE_AND_PRECISION_EXPECT_FAIL(from, to, in, out) \
  try { \
    from a(in); \
    to b; \
    size_t s(0); \
    int p(0); \
    convert(a, b, s, p); \
    UNIT_FAIL("convert must not fail"); \
  } catch (std::bad_cast &) { \
  }

template < class T, class U >
void
convert(const T &, U &, size_t,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<U, char*>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  throw std::bad_cast();
}
*/
void
ConvertTestUnit::convert_to_bool()
{
  CONVERT_EXPECT_SUCCESS               (bool, true, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (bool, true, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (bool, true, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (bool, true, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(bool, true, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (char, 'c', bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (char, 'c', bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (char, 'c', bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (char, 'c', bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(char, 'c', bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (short, -99, bool, false, convert_strict);
  CONVERT_EXPECT_SUCCESS               (short, -99, bool, false, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (short, -99, bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, bool, false, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, bool, false, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (short, -99, bool, false, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, -99, bool, false, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, -99, bool, false, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(short, -99, bool, false, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (int, -99, bool, false, convert_strict);
  CONVERT_EXPECT_SUCCESS               (int, -99, bool, false, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (int, -99, bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, bool, false, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, bool, false, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (int, -99, bool, false, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, -99, bool, false, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, -99, bool, false, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(int, -99, bool, false, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (long, -99, bool, false, convert_strict);
  CONVERT_EXPECT_SUCCESS               (long, -99, bool, false, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (long, -99, bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, bool, false, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, bool, false, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (long, -99, bool, false, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, -99, bool, false, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, -99, bool, false, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(long, -99, bool, false, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (unsigned char, 'c', bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (unsigned char, 'c', bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (unsigned char, 'c', bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned char, 'c', bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned char, 'c', bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (unsigned short, 99, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (unsigned short, 99, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (unsigned short, 99, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned short, 99, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned short, 99, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (unsigned int, 99, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (unsigned int, 99, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (unsigned int, 99, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned int, 99, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned int, 99, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (unsigned long, 99, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (unsigned long, 99, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (unsigned long, 99, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (unsigned long, 99, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(unsigned long, 99, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (float, 99.34567f, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (float, 99.34567f, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (float, 99.34567f, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.34567f, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.34567f, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (float, 99.34567f, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.34567f, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.34567f, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(float, 99.34567f, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (double, 99.34567f, bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (double, 99.34567f, bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (double, 99.34567f, bool, true, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.34567f, bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.34567f, bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (double, 99.34567f, bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.34567f, bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.34567f, bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(double, 99.34567f, bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (const char*, "99", bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (const char*, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (const char*, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_SUCCESS               (const char*, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (const char*, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (const char*, "99", bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(const char*, "99", bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (string, "99", bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (string, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (string, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_SUCCESS               (string, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (string, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (string, "99", bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(string, "99", bool, true, 256, 3, convert_weak);

  CONVERT_EXPECT_FAILURE               (varchar<16>, "99", bool, true, convert_strict);
  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (varchar<16>, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_SUCCESS               (varchar<16>, "99", bool, true, convert_fitting);
  CONVERT_EXPECT_SUCCESS               (varchar<16>, "hello", bool, false, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", bool, true, 256, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", bool, true, 256, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE          (varchar<16>, "99", bool, true, 256, convert_weak);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", bool, true, 256, 3, convert_strict);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", bool, true, 256, 3, convert_fitting);
  CONVERT_EXPECT_FAILURE_SIZE_PRECISION(varchar<16>, "99", bool, true, 256, 3, convert_weak);
}

void
ConvertTestUnit::convert_to_char()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_short()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_int()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_long()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_unsigned_char()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_unsigned_short()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_unsigned_int()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_unsigned_long()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_float()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_double()
{
  /*
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
  */
}

void
ConvertTestUnit::convert_to_char_pointer()
{
  /*
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
  CONVERT_ARRAY_EXPECT_SUCCESS(double, char, -99.34f, "-99.34");
  CONVERT_ARRAY_EXPECT_SUCCESS(const char*, char, "99", "99");
  CONVERT_ARRAY_EXPECT_SUCCESS(std::string, char, "99", "99");
  // TODO: add varchar converts
  CONVERT_ARRAY_EXPECT_SUCCESS(varchar<8>, char, "99", "99");
  */
}

void
ConvertTestUnit::convert_to_string()
{
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
