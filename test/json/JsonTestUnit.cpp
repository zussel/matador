#include "JsonTestUnit.hpp"

#include "json/json_object.hpp"
#include "json/json_number.hpp"
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
  
//  cout << "\n" << obj << "\n";

  UNIT_ASSERT_EQUAL(out.str(), result, "result isn't as expected");
}

void JsonTestUnit::string_test()
{
  string str("{ \"string\" : \"aa\tb\nc\u0a23d\"}");
  
  istringstream sin(str);

  json_object obj;

  sin >> obj;

  stringstream out;
  out << obj;
  
  cout << "\n" << obj << "\n";
  
}


void JsonTestUnit::number_test()
{
  {
    string str("{ \"number\" : 1.99998E+10 }");
    
    istringstream sin(str);
    
    json_object obj;
    
    sin >> obj;
    
    cout << "\n" << obj << "\n";

    json_number numb = obj["number"];
    
    cout << "value of number is: " << numb << "\n";
  }
}

void JsonTestUnit::create_test()
{
  /*
  json_value root(new json_object);
  
  root["number"] = 5.45;
  root["array"] = json_array();
  root["array"][0] = "hello world";
  root["array"][1] = false;
  root["array"][2] = true;
  root["array"][3] = json_null();
  */
}

void JsonTestUnit::access_test()
{
  /*
  if (root["number"].get<long>() == 5000) {
  }
  
  std::string value = root["array"][0].get<string>();
  */
}
