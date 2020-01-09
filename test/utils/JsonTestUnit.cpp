#include "JsonTestUnit.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"

using namespace matador;

JsonTestUnit::JsonTestUnit()
  : unit_test("json", "json test")
{
  add_test("simple", std::bind(&JsonTestUnit::test_simple, this), "test simple json");
  add_test("parser", std::bind(&JsonTestUnit::test_parser, this), "test json parser");
}

void JsonTestUnit::test_simple()
{
  json jn;

  UNIT_ASSERT_TRUE(jn.is_null());

  json ji(7);
  json jb(true);
  json jd(3.5);
  json js("hallo");

  int i = ji.as<int>();
  UNIT_ASSERT_TRUE(ji.is_number());
  UNIT_ASSERT_EQUAL(7, i);
  bool b = jb.as<bool>();
  UNIT_ASSERT_TRUE(jb.is_boolean());
  UNIT_ASSERT_TRUE(b);
  double d = jd.as<double>();
  UNIT_ASSERT_TRUE(jd.is_number());
  UNIT_ASSERT_EQUAL(3.5, d);
  auto str = js.as<std::string>();
  UNIT_ASSERT_TRUE(js.is_string());
  UNIT_ASSERT_EQUAL("hallo", str);

  UNIT_ASSERT_FALSE(js.is_object());

  js["name"] = "hans";
  js["age"] = 67;
  js["weight"] = 78.4;
  js["male"] = true;

  UNIT_ASSERT_TRUE(js.is_object());

  UNIT_ASSERT_EQUAL("hans", js["name"].as<std::string>());
  UNIT_ASSERT_EQUAL(67, js["age"].as<int>());
  UNIT_ASSERT_EQUAL(78.4f, js["weight"].as<float>());
  UNIT_ASSERT_TRUE(js["male"].as<bool>());

  json family = json::object();

  UNIT_ASSERT_TRUE(family.is_object());

  // js ist copied to key 'father'
  family["father"] = js;

  UNIT_ASSERT_EQUAL("hans", family["father"]["name"].as<std::string>());

  family["mother"] = json::object();

  ji.push_back(9);

  UNIT_ASSERT_TRUE(ji.is_array());
  UNIT_ASSERT_EQUAL(1UL, ji.size());

  auto ii = ji.at<int>(0);

  UNIT_ASSERT_EQUAL(9, ii);

  ji.push_back(jb);

  UNIT_ASSERT_EQUAL(2UL, ji.size());

  auto bb = ji.at<bool>(1);

  UNIT_ASSERT_TRUE(bb);

  json jv = {"hallo", 9, 4.6, false, json::array(), json::object()};

  UNIT_ASSERT_TRUE(jv.is_array());
  UNIT_ASSERT_EQUAL(6UL, jv.size());
  UNIT_ASSERT_EQUAL("hallo", jv.at<std::string>(0));
  UNIT_ASSERT_EQUAL(4.6f, jv[2].as<float>());
  UNIT_ASSERT_TRUE(jv[4].is_array());
  UNIT_ASSERT_EQUAL(0UL, jv[4].size());

  jv.erase(2);

  UNIT_ASSERT_EQUAL(5UL, jv.size());
  UNIT_ASSERT_FALSE(jv[2].as<bool>());

  json jo = {{
               "hallo", "welt"
             }};

//  std::cout << "\n";
//  std::cout << jv << "\n";
//  std::cout << jo << "\n";
//  std::cout << js << "\n";

  UNIT_ASSERT_TRUE(jo.is_object());
  UNIT_ASSERT_EQUAL(1UL, jo.size());
/*
  auto it = js.begin();

  std::cout << "iterator value: " << *it << "\n";

  for (auto &val : js) {
    std::cout << "object value: " << val << "\n";
  }

  for (auto &val : jv) {
    std::cout << "array value: " << val << "\n";
  }

  for (auto &val : jd) {
    std::cout << "builtin value: " << val << "\n";
  }
  */
  json jnull;

  jnull = json::object();

  UNIT_ASSERT_FALSE(jnull.is_number());
}

void JsonTestUnit::test_parser()
{
  json_parser parser;

  json j = parser.parse(R"(           {      "text" :       "hello world!"   ,     "bool" : false   })");

  std::string result(R"({"bool": false, "text": "hello world!"})");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"(           {      "text" :       "hello world!",     "bool" : false, "array" :  [   null, false, -5.66667,123456789    ], "serializable" :      { "found": true}      })");

  result = R"({"serializable": {"found": true}, "array": [null, false, -5.66667, 123456789], "text": "hello world!", "bool": false})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j =  parser.parse(R"(   {       "serializable" : { "found" : true }    }    )");

  result = R"({"serializable": {"found": true}})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));
}
