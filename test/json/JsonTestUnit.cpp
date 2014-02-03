#include "JsonTestUnit.hpp"

#include "json/json.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace oos;

JsonTestUnit::JsonTestUnit()
  : unit_test("json", "json test unit")
{
  add_test("simple", std::tr1::bind(&JsonTestUnit::simple_test, this), "simple json test");
  add_test("bool", std::tr1::bind(&JsonTestUnit::bool_test, this), "bool json test");
  add_test("string", std::tr1::bind(&JsonTestUnit::string_test, this), "string json test");
  add_test("number", std::tr1::bind(&JsonTestUnit::number_test, this), "number json test");
  add_test("array", std::tr1::bind(&JsonTestUnit::array_test, this), "array json test");
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

void JsonTestUnit::bool_test()
{
  json_bool b;
  
  bool v = b.value();

  UNIT_ASSERT_EQUAL(v, true, "values are not equal");
  
  b.value(false);

  UNIT_ASSERT_EQUAL(b.value(), false, "values are not equal");

  bool failed = true;
  try {
    json_bool b2(json_value("hallo"));
  } catch (std::logic_error &ex) {
    failed = false;
  }
  UNIT_ASSERT_EQUAL(failed, false, "invalid json_bool initialization");
  
  std::stringstream in;
  
  in << "true";
  
  bool ret = b.parse(in);

  UNIT_ASSERT_EQUAL(ret, true, "parsing unexpectly failed");
  UNIT_ASSERT_EQUAL(v, true, "values are not equal");

  in.str("");
  in << "false";
  
  ret = b.parse(in);
  
  v = b.value();

  UNIT_ASSERT_EQUAL(ret, true, "parsing unexpectly failed");
  UNIT_ASSERT_EQUAL(v, false, "values are not equal");
  
  in.str("");
  in.clear();
  
  ret = b.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
  
  in.str("");
  in.clear();
  in << "tro";
  
  ret = b.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");

  in.str("");
  in.clear();
  in << "tru";
  
  ret = b.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");

  in.str("");
  in.clear();
  in << "falz";
  
  ret = b.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");

  in.str("");
  in.clear();
  in << "fals";
  
  ret = b.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
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

void JsonTestUnit::array_test()
{
  json_array a;
  
  bool failed = true;
  try {
    json_array a2(json_value("hallo"));
  } catch (std::logic_error &ex) {
    failed = false;
  }
  UNIT_ASSERT_EQUAL(failed, false, "invalid json_array initialization");

  UNIT_ASSERT_TRUE(a.empty(), "json array must be empty");

  a.push_back(true);
  
  json_bool b = a[0];
  
  a.push_back("hallo");
  
  json_value& v = a[1];

  UNIT_ASSERT_TRUE(v.is_type<json_string>(), "values must be equal");

  const json_value& cv = a[0];
  
  UNIT_ASSERT_TRUE(cv.is_type<json_bool>(), "values must be equal");

  size_t s = a.size();
  
  UNIT_ASSERT_TRUE(s == 2, "size of json array must be 2");
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

  UNIT_ASSERT_TRUE(root.size() == 2, "json object must contain 2 elements");

  json_object obj = root;

  UNIT_ASSERT_TRUE(obj.size() == 2, "json object must contain 2 elements");

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

  UNIT_ASSERT_TRUE(ary.size() == 5, "json array must contain 5 elements");

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
