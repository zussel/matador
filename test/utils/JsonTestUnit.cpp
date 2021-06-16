#include <list>
#include <unordered_set>
#include <sstream>

#include "JsonTestUnit.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"

using namespace matador;

JsonTestUnit::JsonTestUnit()
  : unit_test("json", "json test")
{
  add_test("simple", [this] { test_simple(); }, "test simple json");
  add_test("access", [this] { test_access(); }, "test json access");
  add_test("compare", [this] { test_compare(); }, "test json compare");
  add_test("parser", [this] { test_parser(); }, "test json parser");
}

void JsonTestUnit::test_simple()
{
  json jn;
  UNIT_ASSERT_TRUE(jn.is_null());

  json jts(json::e_string);
  UNIT_ASSERT_TRUE(jts.is_string());

  json jtr(json::e_real);
  UNIT_ASSERT_TRUE(jtr.is_real());
  UNIT_ASSERT_TRUE(jtr.is_number());

  json ji(7);
  json jb(true);
  json jd(3.5);
  json js("hallo");
  json jnill(nullptr);

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
  ji.push_back("hallo");

  UNIT_ASSERT_TRUE(ji.is_array());
  UNIT_ASSERT_EQUAL(2UL, ji.size());
  UNIT_ASSERT_FALSE(ji.empty());

  auto jib = ji.back();
  UNIT_EXPECT_EQUAL("hallo", jib.as<std::string>());

  // invalid index
  UNIT_ASSERT_EXCEPTION(ji[2], std::logic_error, "index out of bounds");

  auto jii = ji[0];
  UNIT_EXPECT_EQUAL(9, jii.as<int>());

  // not an array
  UNIT_ASSERT_EQUAL(1UL, jii.size());
  UNIT_ASSERT_FALSE(jii.empty());
  //UNIT_ASSERT_EXCEPTION(jii.size(), std::logic_error, "type doesn't have size()");
  //UNIT_ASSERT_EXCEPTION(jii.empty(), std::logic_error, "type doesn't have empty()");

  auto ii = ji.at<int>(0);
  UNIT_ASSERT_EQUAL(9, ii);

  ji.push_back(jb);

  UNIT_ASSERT_EQUAL(3UL, ji.size());

  auto bb = ji.at<bool>(2);
  UNIT_ASSERT_TRUE(bb);

  UNIT_ASSERT_EXCEPTION(ji.at<bool>(1), std::logic_error, "wrong type; couldn't cast");

  const json jac = {1,2,true};

  UNIT_ASSERT_EQUAL(3UL, jac.size());
  const auto &jacb = jac.back();
  UNIT_ASSERT_TRUE(jacb.as<bool>());

  auto it = ji.begin();
  auto iend = ji.end();
  size_t count = 0;
  while (it != iend) {
    ++count;
    ++it;
  }
  UNIT_ASSERT_EQUAL(3UL, count);

  json jv = {"hallo", 9, 4.6, false, json::array(), json::object()};

  UNIT_ASSERT_TRUE(jv.is_array());
  UNIT_ASSERT_EQUAL(6UL, jv.size());
  UNIT_ASSERT_EQUAL("hallo", jv.at<std::string>(0));
  UNIT_ASSERT_EQUAL(4.6f, jv[2].as<float>());
  UNIT_ASSERT_TRUE(jv[4].is_array());
  UNIT_ASSERT_EQUAL(0UL, jv[4].size());

  jv.erase("hallo");
  UNIT_ASSERT_FALSE(jv.empty());
  UNIT_ASSERT_EQUAL(6UL, jv.size());

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
  UNIT_ASSERT_FALSE(jo.empty());
  UNIT_ASSERT_EQUAL(1UL, jo.size());

  UNIT_ASSERT_EXCEPTION(jo.back(), std::logic_error, "type doesn't have back()");

  count = 0;
  it = jo.begin();
  iend = jo.end();
  while (it != iend) {
    ++count;
    ++it;
  }
  UNIT_ASSERT_EQUAL(1UL, count);

  jo.erase(1);
  UNIT_ASSERT_FALSE(jo.empty());
  UNIT_ASSERT_EQUAL(1UL, jo.size());

  jo.erase("hallo");
  UNIT_ASSERT_TRUE(jo.empty());
  UNIT_ASSERT_EQUAL(0UL, jo.size());

  const json jco = json::object();
  UNIT_ASSERT_EXCEPTION(
  const auto &r = jco.back(); r.empty();,
  std::logic_error,
  "type doesn't have back()"
  );



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

void JsonTestUnit::test_access()
{
  json j = {
    { "person", {
                  { "name", "harry" },
                  { "age", 30 }
    }}
  };

  auto age = j.at_path("person.age");

  UNIT_ASSERT_TRUE(age.is_number());
  UNIT_ASSERT_TRUE(age == 30);

  auto self = j.at_path("");

  UNIT_ASSERT_EQUAL(self, j);

  const json cj = { { "int", 8 } };

  auto val = cj.get("int");

  UNIT_ASSERT_TRUE(val.is_number());
  UNIT_ASSERT_TRUE(val == 8);

  val = cj["int"];

  UNIT_ASSERT_TRUE(val.is_number());
  UNIT_ASSERT_TRUE(val == 8);

  json obj = {
    { "type", "object" },
    { "valid", true },
    { "numbers", { 1, 2, 3} },
    { "nope", nullptr }
  };

  std::stringstream out;

  out << obj;

  auto str = obj.str();

  UNIT_ASSERT_EQUAL("{\"nope\": null, \"numbers\": [1, 2, 3], \"type\": \"object\", \"valid\": true}", str.c_str());
}

void JsonTestUnit::test_compare()
{
  json a = 7;

  UNIT_ASSERT_TRUE(a < 9);
  UNIT_ASSERT_FALSE(9 < a);

  json f = 7.2;

  UNIT_ASSERT_TRUE(a < f);
  UNIT_ASSERT_FALSE(f < a);

  json s = "hello";

  UNIT_ASSERT_TRUE(s == "hello");
  UNIT_ASSERT_FALSE(s != "hello");
  UNIT_ASSERT_FALSE(s == "world");
  UNIT_ASSERT_TRUE(s != "world");

  json b("hello");

  UNIT_ASSERT_TRUE(s == b);

  json_parser parser;
  json c = parser.parse("\"hello\"");

  UNIT_ASSERT_TRUE(s == c);

  UNIT_ASSERT_FALSE(f == s);

  json jnull = json();

  UNIT_ASSERT_TRUE(jnull.is_null());

  UNIT_ASSERT_TRUE(jnull == json());

  UNIT_ASSERT_FALSE(jnull < json());
  UNIT_ASSERT_TRUE(json(3) < json(7));
  UNIT_ASSERT_TRUE(json(3.7) < json(7.8));
  UNIT_ASSERT_TRUE(json(3) < json(7.8));
  UNIT_ASSERT_TRUE(json(3.7) < json(7));
  UNIT_ASSERT_TRUE(json(false) < json(true));
  UNIT_ASSERT_TRUE(json("aa") < json("bb"));

  json o1 = { { "int", 7 }};
  json o2 = { { "int", 9 }};

  UNIT_ASSERT_TRUE(o1 < o2);

  json a1 = { 1,2,3 };
  json a2 = { 7,8,9 };

  UNIT_ASSERT_TRUE(a1 < a2);

  UNIT_ASSERT_TRUE(json(true) < json("aaa"));
}

void JsonTestUnit::test_parser()
{
  json_parser parser;

  json j = parser.parse(R"(           {      "text" :       "hello world!"   ,     "bool" : false   }   )");

  std::string result(R"({"bool": false, "text": "hello world!"})");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"( { } )");

  result = R"({})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"( [ ] )");

  result = R"([])";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"(  { "real"  : -5.66667, "number"  :   -123456789 })");

  result = R"({"number": -123456789, "real": -5.666670})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"(           {      "text" :       "hello world!",     "bool" : false, "array" :  [   null, false, -5.66667,123456789    ], "serializable" :      { "found": true}      })");

  result = R"({"array": [null, false, -5.666670, 123456789], "bool": false, "serializable": {"found": true}, "text": "hello world!"})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j =  parser.parse(R"(   {       "serializable" : { "found" : true }    }    )");

  result = R"({"serializable": {"found": true}})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  // nested array
  j = parser.parse(R"( [  {  "name":   "hello" }    ]  )");

  result = R"([{"name": "hello"}])";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"( [  [   "hello",   null     ]    ]  )");

  result = R"([["hello", null]])";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  std::string str(R"(           {      "text" :       "hello world!"   ,     "bool" : false   }   )");

  j = parser.parse(str);

  result = R"({"bool": false, "text": "hello world!"})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse("7");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_TRUE(j.is_number());

  j = parser.parse("7.67");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_TRUE(j.is_number());

  j = parser.parse("true");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_TRUE(j.is_boolean());

  j = parser.parse("\"hallo\"");

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_TRUE(j.is_string());
  UNIT_ASSERT_EQUAL("hallo", j.as<std::string>());

  j = parser.parse("null");

  UNIT_ASSERT_TRUE(j.is_null());
}

