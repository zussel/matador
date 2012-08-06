#include "JsonTestUnit.hpp"

#include "json/json_object.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace oos;

JsonTestUnit::JsonTestUnit()
  : unit_test("json test unit")
{
  add_test("simple", std::tr1::bind(&JsonTestUnit::simple_test, this), "simple json test");
}

JsonTestUnit::~JsonTestUnit()
{
}

void JsonTestUnit::simple_test()
{
  string str("           {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false ]      }");
  
  string result("{ \"array\" : [ null, false ], \"bool\" : false, \"text\" : \"hello world!\" }");
  istringstream sin(str);

  json_object obj;

  sin >> obj;

  stringstream out;
  out << obj;
  
  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}
