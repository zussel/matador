//
// Created by sascha on 27.02.20.
//

#include "JsonMapperTestUnit.hpp"

#include "matador/utils/varchar.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/utils/json_mapper.hpp"
#include "../person.hpp"

#include <set>
#include <unordered_set>

using namespace matador;

JsonMapperTestUnit::JsonMapperTestUnit()
  : unit_test("json_mapper", "json test")
{
  add_test("fields", std::bind(&JsonMapperTestUnit::test_fields, this), "test mapping object with builtin fields");
  add_test("array_builtins", std::bind(&JsonMapperTestUnit::test_array_of_builtins, this), "test mapping array of builtin fields");
  add_test("array_objects", std::bind(&JsonMapperTestUnit::test_array_of_objects, this), "test mapping array of objects");
  add_test("nested_object", std::bind(&JsonMapperTestUnit::test_nested_object, this), "test mapping nested object");
  add_test("complex", std::bind(&JsonMapperTestUnit::test_complex, this), "test mapping complex object");
  add_test("failures", std::bind(&JsonMapperTestUnit::test_failure, this), "test mapping failures");
  add_test("false_types", std::bind(&JsonMapperTestUnit::test_false_types, this), "test mapping with false types");
  add_test("special_chars", std::bind(&JsonMapperTestUnit::test_special_chars, this), "test mapping special characters");
}

struct bounding_box
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

struct dto
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
  bounding_box dimension;
  std::vector<bounding_box> dimensions;

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

void JsonMapperTestUnit::test_fields()
{
  json_mapper<dto> mapper;

  auto p = mapper.object_from_string(R"(  {
"id":  "george@mail.net",
"name": "george",
"birthday": "1987-09-27",
"created": "2020-02-03 13:34:23",
"flag": false,
"height": 183
} )");

  date b(27, 9, 1987);

  UNIT_ASSERT_EQUAL("george@mail.net", p.id.value());
  UNIT_ASSERT_EQUAL("george", p.name);
  UNIT_ASSERT_EQUAL(b, p.birthday);
  UNIT_ASSERT_EQUAL(183L, p.height);
  UNIT_ASSERT_FALSE(p.flag);
}

void JsonMapperTestUnit::test_array_of_builtins()
{
  json_mapper<dto> mapper;

  auto p = mapper.object_from_string(R"(  {
"doubles": [1.2, 3.5, 6.9],
"bits": [true, false, true],
"names": ["hans", "clara", "james"],
"values": [11, 12, 13]
} )");

  UNIT_ASSERT_EQUAL(3U, p.doubles.size());
  UNIT_ASSERT_EQUAL(3U, p.bits.size());
  auto it = p.bits.begin();
  UNIT_EXPECT_TRUE(*(it++));
  UNIT_ASSERT_FALSE(*(it++));
  UNIT_EXPECT_TRUE(*(it++));
  UNIT_ASSERT_EQUAL(3U, p.names.size());
  UNIT_ASSERT_EQUAL(3U, p.values.size());
}

void JsonMapperTestUnit::test_array_of_objects()
{
  json_mapper<bounding_box> sub_mapper;

  auto subs = sub_mapper.array_from_string(R"( [ { "length": 200, "width":  300, "height":   100 }, { "length": 900, "width":  800, "height":   700 } ] )");

  UNIT_ASSERT_EQUAL(2UL, subs.size());

  auto &item = subs[0];

  UNIT_ASSERT_EQUAL(200L, item.length);
  UNIT_ASSERT_EQUAL(300L, item.width);
  UNIT_ASSERT_EQUAL(100L, item.height);

  item = subs[1];

  UNIT_ASSERT_EQUAL(900L, item.length);
  UNIT_ASSERT_EQUAL(800L, item.width);
  UNIT_ASSERT_EQUAL(700L, item.height);
}

void JsonMapperTestUnit::test_nested_object()
{
  json_mapper<dto> mapper;

  auto p = mapper.object_from_string(R"(  {
"dimension": { "length": 200, "width":  300, "height":   100 }
} )");

  UNIT_EXPECT_EQUAL(200, p.dimension.length);
  UNIT_EXPECT_EQUAL(300, p.dimension.width);
  UNIT_EXPECT_EQUAL(100, p.dimension.height);
}

void JsonMapperTestUnit::test_complex()
{
  json_mapper<dto> mapper;

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
"dimension": { "length": 200, "width":  300, "height":   100 },
"dimensions": [{ "length": 200, "width":  300, "height":   100 }, { "length": 900, "width":  800, "height":   700 }]
} )");

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
  UNIT_EXPECT_EQUAL(2UL, p.dimensions.size());
}

void JsonMapperTestUnit::test_failure()
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

void JsonMapperTestUnit::test_false_types()
{
  json_mapper<dto> mapper;

  // check false types
  auto p = mapper.object_from_string(R"(  {     "id":  9, "name": true,
"birthday": false, "created": false, "flag": 2.3, "height": "wrong",
"doubles": false,
"bits": "hallo",
"names": false,
"values": false
} )");

  date b(27, 9, 1987);

  UNIT_EXPECT_EQUAL("", p.id.value());
  UNIT_EXPECT_EQUAL("", p.name);
  UNIT_EXPECT_FALSE(p.flag);
  UNIT_EXPECT_FALSE(b == p.birthday);
  UNIT_EXPECT_EQUAL(0L, p.height);
  UNIT_EXPECT_EQUAL(0U, p.doubles.size());
  UNIT_EXPECT_EQUAL(0U, p.bits.size());
  UNIT_EXPECT_EQUAL(0U, p.names.size());
  UNIT_EXPECT_EQUAL(0U, p.values.size());
}

void JsonMapperTestUnit::test_special_chars()
{
  json_mapper<person> pmapper;

  person pp(pmapper.object_from_string(R"({ "name": "\r\ferik\tder\nwikinger\b\u0085"})"));

  UNIT_EXPECT_EQUAL("\r\ferik\tder\nwikinger\b\\u0085", pp.name());
}
