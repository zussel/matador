#include "JsonTestUnit.hpp"

#include "json/json_object.hpp"
#include "json/json_number.hpp"
#include "json/json_bool.hpp"
#include "json/json_array.hpp"
#include "json/json_null.hpp"
#include "json/json_bool.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace oos;

JsonTestUnit::JsonTestUnit()
  : unit_test("json test unit")
{
  add_test("simple", std::tr1::bind(&JsonTestUnit::simple_test, this), "simple json test");
  add_test("string", std::tr1::bind(&JsonTestUnit::string_test, this), "string json test");
  add_test("number", std::tr1::bind(&JsonTestUnit::number_test, this), "number json test");
  add_test("create", std::tr1::bind(&JsonTestUnit::create_test, this), "create json test");
  add_test("access", std::tr1::bind(&JsonTestUnit::access_test, this), "access json test");
}

JsonTestUnit::~JsonTestUnit()
{
}

void JsonTestUnit::simple_test()
{
  
  string str("           {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false, -5.66667 ]      }");
  
  string result("{ \"array\" : [ null, false, -5.66667 ], \"bool\" : false, \"text\" : \"hello world!\" }");
  istringstream sin(str);

  json_object obj;

  sin >> obj;

  stringstream out;
  out << obj;
  
  //cout << "\n" << obj << "\n";

  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}

void JsonTestUnit::string_test()
{
  string str("{ \"string\" : \"aa\tb\nc\u00ffd\"}");
  
  string result("aa\tb\nc\u00ffd");

  istringstream sin(str);

  json_object obj;

  sin >> obj;

  json_string numb = obj["string"];

  UNIT_ASSERT_EQUAL(numb.value(), result, "values are not equal");
}


void JsonTestUnit::number_test()
{
  {
    string str("{ \"number\" : 1.99998E+10 }");
    
    istringstream sin(str);
    
    json_object obj;
    
    sin >> obj;
    
    json_number numb = obj["number"];
    
    UNIT_ASSERT_EQUAL(numb.value(), 1.99998E+10, "values are not equal");
  }
}

void JsonTestUnit::create_test()
{
  json_object root;
  
  root["number"] = 5.45;
  root["array"] = new json_array(4);
  root["array"][0] = "hello world";
  root["array"][1] = false;
  root["array"][2] = true;
  root["array"][3] = new json_null;
  root["array"].push_back(5);

  string result("{ \"array\" : [ \"hello world\", false, true, null, 5 ], \"number\" : 5.45 }");

  stringstream out;
  out << root;
  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}

void JsonTestUnit::access_test()
{
  json_object root;
  
  root["number"] = 5.45;
  root["array"] = new json_array(4);
  root["array"][0] = "hello world";
  root["array"][1] = false;
  root["array"][2] = true;
  root["array"][3] = new json_null;
  root["array"].push_back(5);

  UNIT_ASSERT_EQUAL(root.size(), 2, "json object must contain 2 elements");

  /*
  if (root["number"].get<long>() == 5000) {
  }
  
  std::string value = root["array"][0].get<string>();
  */

  json_object obj = root;

  UNIT_ASSERT_EQUAL(obj.size(), 2, "json object must contain 2 elements");

  json_number numb = root["number"];

  UNIT_ASSERT_EQUAL(numb.value(), 5.45, "number values are not the same");

  json_array ary = root["array"];

  UNIT_ASSERT_EQUAL(ary.size(), 5, "json array must contain 5 elements");

  json_array::const_iterator first = ary.begin();
  json_array::const_iterator last = ary.end();

  while (first != last) {
    ++first;
  }
}
