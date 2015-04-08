#include "JsonTestUnit.hpp"

#include "json/json.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;
using namespace oos;

JsonTestUnit::JsonTestUnit()
  : unit_test("json", "json test unit")
{
  add_test("simple", std::bind(&JsonTestUnit::simple_test, this), "simple json test");
  add_test("invalid", std::bind(&JsonTestUnit::invalid_test, this), "invalid json test");
  add_test("null", std::bind(&JsonTestUnit::null_test, this), "null json test");
  add_test("bool", std::bind(&JsonTestUnit::bool_test, this), "bool json test");
  add_test("string", std::bind(&JsonTestUnit::string_test, this), "string json test");
  add_test("number", std::bind(&JsonTestUnit::number_test, this), "number json test");
  add_test("array", std::bind(&JsonTestUnit::array_test, this), "array json test");
  add_test("create", std::bind(&JsonTestUnit::create_test, this), "create json test");
  add_test("access", std::bind(&JsonTestUnit::access_test, this), "access json test");
  add_test("parser", std::bind(&JsonTestUnit::parser_test, this), "parser json test");
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

  UNIT_ASSERT_EQUAL(obj.size(), (size_t)3, "size of json object must be 3");
  UNIT_ASSERT_TRUE(obj.contains("text"), "object must contain 'text' attribute");

  int count = 0;
  for_each(obj.begin(), obj.end(), [&](const json_object::value_type &) {
    ++count;
  });
  UNIT_ASSERT_EQUAL(count, 3, "object must contain 3 elements");

  stringstream out;
  out << obj;
  
  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");

  obj.clear();
  UNIT_ASSERT_TRUE(obj.empty(), "object must be empty");

  obj.insert("name", "jon");
  UNIT_ASSERT_TRUE(obj.contains("name"), "object must contain 'name' attribute");

  str = "   [ {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false, -5.66667 ]      } ]               ";
  result = "[ { \"array\" : [ null, false, -5.66667 ], \"bool\" : false, \"text\" : \"hello world!\" } ]";
  sin.clear();
  sin.str(str);

  json_array ary;

  sin >> ary;

  out.str("");
  out << ary;

  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}

void JsonTestUnit::invalid_test()
{
  string str("      xxx     {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false, -5.66667 ]      }");
  istringstream sin(str);
  json_object obj;

  UNIT_ASSERT_EXCEPTION(sin >> obj, std::logic_error, "root must be either array '[]' or object '{}'", "shouldn't create json object from invalid json");

  str = "           {      \"text\" :       \"hello world!\",     \"bool\" : false, \"array\" :  [   null, false, -5.66667 ]      } xxxx ";
  sin.clear();
  sin.str(str);

  UNIT_ASSERT_EXCEPTION(sin >> obj, std::logic_error, "no characters are allowed after closed root node", "shouldn't create json object from invalid json");
}

void JsonTestUnit::null_test()
{
  json_null n;

  std::stringstream in;
  
  in << "null";
  
  bool ret = n.parse(in);

  UNIT_ASSERT_EQUAL(ret, true, "parsing unexpectly failed");
  
  in.str("");
  in.clear();
  
  ret = n.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
  
  in.str("");
  in.clear();
  in << "nut";
  
  ret = n.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
  
  in.str("");
  in.clear();
  in << "nul";
  
  ret = n.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
  
  in.str("");
  in.clear();
  in << "full";
  
  ret = n.parse(in);
  UNIT_ASSERT_EQUAL(ret, false, "parsing unexpectly successed");
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

    json_number numb2(numb);

    UNIT_ASSERT_EQUAL(numb2.value(), 1.99998E+10, "values are not equal");

    json_number numb3;

    numb2 = 5.2;

    UNIT_ASSERT_EQUAL(numb2.value(), 5.2, "values are not equal");

    UNIT_ASSERT_TRUE(numb2 < numb, "numb2 must be less than numb");

    numb3 = numb2;

    UNIT_ASSERT_EQUAL(numb3.value(), 5.2, "values are not equal");

    numb3 = obj["number"];

    UNIT_ASSERT_EQUAL(numb3.value(), 1.99998E+10, "values are not equal");

    numb2.value(-1.223);

    UNIT_ASSERT_EQUAL(numb2.value(), -1.223, "values are not equal");
  }
}

void JsonTestUnit::array_test()
{
  json_array a;

  UNIT_ASSERT_EXCEPTION(json_array a2(json_value("hallo")), std::logic_error, "json_value isn't of type json_array", "shouldn't initialize ");

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
  
  // check clear and empty
  a.clear();  
  UNIT_ASSERT_TRUE(a.empty(), "json array must be empty");

  // check parsing
  stringstream in;
  in << "  [ 1, true, \"hallo\"]";
  
  bool ret = a.parse(in);
  UNIT_ASSERT_EQUAL(ret, true, "parsing unexpectly failed");
  UNIT_ASSERT_EQUAL(a.size(), (size_t)3, "values are not equal");
  
  json_value jv(new json_array);
  jv.push_back(1);
  jv.push_back(2);
  a = jv;
  UNIT_ASSERT_EQUAL(a.size(), (size_t)2, "values are not equal");
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
