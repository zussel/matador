#include "ConvertTestUnit.hpp"

#include "tools/convert.hpp"
#include "tools/varchar.hpp"

#include <iostream>
#include <string>

using namespace oos;

using std::cout;
using std::string;

ConvertTestUnit::ConvertTestUnit()
  : unit_test("convert test unit")
{
//  add_test("convert", std::tr1::bind(&ConvertTestUnit::convert_test, this), "convert test");
  add_test("to_char", std::tr1::bind(&ConvertTestUnit::convert_to_char, this), "convert to char test");
}

ConvertTestUnit::~ConvertTestUnit()
{}

void ConvertTestUnit::convert_test()
{
  double dd(0.0);
  
  convert("5.367", dd);
  
  UNIT_ASSERT_EQUAL(dd, 5.367, "unexpected value");

  float f(0.0);
  
  convert("47.11", f);
  
  UNIT_ASSERT_EQUAL(f, 47.11f, "unexpected value");

  int i1(0), i2(3);
  long l1(0);
  short s1(0);
  bool b(true);
  std::string str;
  
  std::string dstr("5.3");
  std::string istr("7");
  std::string b1str("true");
  std::string b2str("0");

  varchar<12> dvar1("18.7");
  varchar<12> dvar2("8.2");

  convert(dvar1, dd);
  convert(dstr, dd);
  convert(istr, i1);
  convert(i2, i1);
  convert(b2str, b);
  convert(i2, l1);
  convert("47.11", dd);
//  convert(l1, s1);
  convert(f, dd);
  convert(b1str, str);
  convert(dvar1, dvar2);
  convert(dstr, l1);
}

void
ConvertTestUnit::convert_to_char()
{
  /*
   * valid convertions from:
   * char
   * short
   * int
   * long
   * unsigned short
   * unsigned int
   * unsigned long
   */

  std::cout << "\n";

  cout << "std::tr1::is_integral<char>::value: " << std::tr1::is_integral<char>::value << "\n";
  cout << "std::tr1::is_integral<short>::value: " << std::tr1::is_integral<short>::value << "\n";
  cout << "std::tr1::is_integral<int>::value: " << std::tr1::is_integral<int>::value << "\n";
  cout << "std::tr1::is_integral<long>::value: " << std::tr1::is_integral<long>::value << "\n";

  char to(0);
  char c = 'c';
  /*
   * char to char
   */
  {
    convert(c, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * short to char
   */
  {
    to = (char)0;
    short from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * int to char
   */
  {
    to = (char)0;
    int from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * long to char
   */
  {
    to = (char)0;
    long from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * unsigned short to char
   */
  {
    to = (char)0;
    unsigned short from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * unsigned int to char
   */
  {
    to = (char)0;
    unsigned int from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  /*
   * unsigned long to char
   */
  {
    to = (char)0;
    unsigned long from = 99;
    convert(from, to);
    UNIT_ASSERT_EQUAL(to, c, "converted character isn't the expected one");
  }
  
  try {
    to = (char)0;
    float from = 99;
    convert(from, to);
    UNIT_FAIL("shouldn't convert float to char");
  } catch (std::bad_cast &ex) {
    // test succeeded
  }
  
  try {
    to = (char)0;
    double from = 99;
    convert(from, to);
    UNIT_FAIL("shouldn't convert double to char");
  } catch (std::bad_cast &ex) {
    // test succeeded
  }
  
  try {
    to = (char)0;
    const char* from = "99";
    convert(from, to);
    UNIT_FAIL("shouldn't convert const char* to char");
  } catch (std::bad_cast &ex) {
    // test succeeded
  }
  
  try {
    to = (char)0;
    std::string from("99");
    convert(from, to);
    UNIT_FAIL("shouldn't convert std::string to char");
  } catch (std::bad_cast &ex) {
    // test succeeded
  }
  
  try {
    to = (char)0;
    varchar<8> from("99");
    convert(from, to);
    UNIT_FAIL("shouldn't convert varchar_base to char");
  } catch (std::bad_cast &ex) {
    // test succeeded
  }
}
