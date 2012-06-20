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
  add_test("convert", std::tr1::bind(&ConvertTestUnit::convert_test, this), "convert test");
}

ConvertTestUnit::~ConvertTestUnit()
{}

void ConvertTestUnit::convert_test()
{
  double d(0.0);
  float f(13.0);
  int i1(0), i2(3);
  long l1(0);
  short s1(0);
  bool b(true);
  std::string str;
  
  std::string dstr("5.3");
  std::string istr("7");
  std::string b1str("true");
  std::string b2str("0");

  varchar<12> dvar("18.7");

  convert(dvar, d);
  convert(dstr, d);
  convert(istr, i1);
  convert(i2, i1);
  convert(b2str, b);
  convert(i2, l1);
//  convert(l1, s1);
  convert(f, d);
  convert(b1str, str);
}
