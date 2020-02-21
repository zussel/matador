#include <set>
#include <list>
#include <unordered_set>

#include "JsonTestUnit.hpp"

#include "../person.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/json_mapper.hpp"
#include "matador/utils/time.hpp"

using namespace matador;

JsonTestUnit::JsonTestUnit()
  : unit_test("json", "json test")
{
  add_test("simple", std::bind(&JsonTestUnit::test_simple, this), "test simple json");
  add_test("parser", std::bind(&JsonTestUnit::test_parser, this), "test json parser");
  add_test("mapper", std::bind(&JsonTestUnit::test_mapper, this), "test json mapper");
  add_test("failures", std::bind(&JsonTestUnit::test_failures, this), "test json mapping failures");
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
  UNIT_ASSERT_EXCEPTION(jii.size(), std::logic_error, "type doesn't have size()");
  UNIT_ASSERT_EXCEPTION(jii.empty(), std::logic_error, "type doesn't have empty()");

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

  result = R"({"number": -123456789, "real": -5.66667})";

  UNIT_ASSERT_FALSE(j.is_null());
  UNIT_ASSERT_EQUAL(result, to_string(j));

  j = parser.parse(R"(           {      "text" :       "hello world!",     "bool" : false, "array" :  [   null, false, -5.66667,123456789    ], "serializable" :      { "found": true}      })");

  result = R"({"array": [null, false, -5.66667, 123456789], "bool": false, "serializable": {"found": true}, "text": "hello world!"})";

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
}

struct json_sub_values
{
  long length = 0;
  long width = 0;
  long height = 0;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("length", length);
    s.serialize("width", width);
    s.serialize("height", height);
  }
};

struct json_values
{
  identifier<varchar<255>> id;
  std::string name;
  date birthday;
  matador::time created;
  bool flag = false;
  long height = 0;
  std::vector<double> doubles;
  std::list<bool> bits;
  std::set<std::string> names;
  std::unordered_set<int> values;
  json_sub_values dimension;
  std::vector<json_sub_values> dimensions;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    s.serialize("birthday", birthday);
    s.serialize("created", created);
    s.serialize("flag", flag);
    s.serialize("height", height);
    s.serialize("doubles", doubles);
    s.serialize("bits", bits);
    s.serialize("names", names);
    s.serialize("values", values);
    s.serialize("dimension", dimension);
    s.serialize("dimensions", dimensions);
  }
};

void JsonTestUnit::test_mapper()
{
  json_mapper<json_values> mapper;

  auto p = mapper.object_from_string(R"(  {
"id":  "george@mail.net",
"name": "george",
"birthday": "1987-09-27",
"created": "2020-02-03 13:34:23",
"flag": false,
"height": 183,
"doubles": [1.2, 3.5, 6.9],
"bits": [true, false, true],
"names": ["hans", "clara", "james"],
"values": [11, 12, 13],
"dimension": { "length": 200, "width":  300, "height":   100 }
} )");

  //  "dimensions": [{ "length": 200, "width":  300, "height":   100 }, { "length": 900, "width":  800, "height":   700 }]


  date b(27, 9, 1987);

  UNIT_EXPECT_EQUAL("george@mail.net", p.id.value());
  UNIT_EXPECT_EQUAL("george", p.name);
  UNIT_EXPECT_EQUAL(b, p.birthday);
  UNIT_EXPECT_EQUAL(183L, p.height);
  UNIT_EXPECT_EQUAL(3U, p.doubles.size());
  UNIT_EXPECT_EQUAL(3U, p.bits.size());
  auto it = p.bits.begin();
  UNIT_EXPECT_TRUE(*(it++));
  UNIT_EXPECT_FALSE(*(it++));
  UNIT_EXPECT_TRUE(*(it++));
  UNIT_EXPECT_EQUAL(3U, p.names.size());
  UNIT_EXPECT_EQUAL(3U, p.values.size());
  UNIT_EXPECT_EQUAL(200, p.dimension.length);
  UNIT_EXPECT_EQUAL(300, p.dimension.width);
  UNIT_EXPECT_EQUAL(100, p.dimension.height);

  // check false types
//  p = mapper.object_from_string(R"(  {     "id":  9, "name": true,
//"birthday": false, "created": false, "flag": 2.3, "height": "wrong",
//"doubles": false,
//"bits": "hallo",
//"names": false,
//"values": false
//} )");
//
//  UNIT_EXPECT_EQUAL("", p.id.value());
//  UNIT_EXPECT_EQUAL("", p.name);
//  UNIT_EXPECT_FALSE(p.flag);
//  UNIT_EXPECT_FALSE(b == p.birthday);
//  UNIT_EXPECT_EQUAL(0L, p.height);
//  UNIT_EXPECT_EQUAL(0U, p.doubles.size());
//  UNIT_EXPECT_EQUAL(0U, p.bits.size());
//  UNIT_EXPECT_EQUAL(0U, p.names.size());
//  UNIT_EXPECT_EQUAL(0U, p.values.size());
//
//  json_mapper<person> pmapper;
//
//  person pp(pmapper.object_from_string(R"({ "name": "\r\ferik\tder\nwikinger\b\u0085"})"));
//
//  UNIT_EXPECT_EQUAL("\r\ferik\tder\nwikinger\b\\u0085", pp.name());
}

void JsonTestUnit::test_failures()
{
  json_mapper<person> mapper;

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(    )"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(    --)"), json_exception, "root must be object '{}'");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  key)"), json_exception, "expected string opening quotes");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key)"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key  )"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key" ; )"), json_exception, "character isn't colon");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key": "value" )"), json_exception, "not a valid object closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key": "value" ]  )"), json_exception, "not a valid object closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  {  "key": "value" } hhh  )"), json_exception, "no characters are allowed after closed root node");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  8)"), json_exception, "invalid json character");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  8 )"), json_exception, "not a valid array closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  8 fff)"), json_exception, "not a valid array closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  12345678901234567890123456789 ]   )"), json_exception, "errno integer error");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  1.79769e+309 ]   )"), json_exception, "errno double error");

  UNIT_ASSERT_EXCEPTION(mapper.array_from_string(R"(  [  1.79769e+308ddd ]   )"), json_exception, "invalid json character");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": t)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": tg)"), json_exception, "invalid character for bool value string");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": f)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": fu)"), json_exception, "invalid character for bool value string");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": w)"), json_exception, "unknown json type");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": n)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.object_from_string(R"(  { "key": na)"), json_exception, "invalid null character");
}

