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
  add_test("convert", std::tr1::bind(&ConvertTestUnit::convert_test, this), "create blob");
}

ConvertTestUnit::~ConvertTestUnit()
{}

void ConvertTestUnit::convert_test()
{
  cout << "\n";

  double d(0.0);
  int i(0), i2(3);
  long l(5);
  std::string str;
  bool b(0);
  
  std::string dstr("5.3");
  std::string istr("7");
  std::string bstr1("true");
  std::string bstr2("1");

  varchar<12> dvar("18.7");

  convert(dvar, d);
  convert(dstr, d);
  convert(istr, i);
  convert(bstr2, b);
  convert(i2, i);
//  convert(i2, l);
//  convert(bstr, str);
}
