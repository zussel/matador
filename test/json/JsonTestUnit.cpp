#include "JsonTestUnit.hpp"

#include "json/json.hpp"

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
  add_test("parser", std::tr1::bind(&JsonTestUnit::parser_test, this), "parser json test");
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

  json_object obj = root;

  UNIT_ASSERT_EQUAL(obj.size(), 2, "json object must contain 2 elements");

  json_number numb = root["number"];

  bool succeeded = false;
  try {
    json_string str =root["number"];
  } catch (std::logic_error &) {
    succeeded = true;
  }
  UNIT_ASSERT_TRUE(succeeded, "json type conversion from number to string must fail");

  UNIT_ASSERT_EQUAL(numb.value(), 5.45, "number values are not the same");

  json_array ary = root["array"];

  UNIT_ASSERT_EQUAL(ary.size(), 5, "json array must contain 5 elements");

  json_array::const_iterator first = ary.begin();
  json_array::const_iterator last = ary.end();

  while (first != last) {
    ++first;
  }
}

void JsonTestUnit::parser_test()
{
  json_parser parser;

  json_value v = parser.parse("           {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false, -5.66667 ], \"object\" :      { \"found\": true}      }");

  string result("{ \"array\" : [ null, false, -5.66667 ], \"bool\" : false, \"object\" : { \"found\" : true }, \"text\" : \"hello world!\" }");

  stringstream out;
  out << v;
  
  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}
