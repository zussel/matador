#include "JsonMapperTestUnit.hpp"

#include "matador/utils/date.hpp"

#include "matador/utils/json_mapper.hpp"

#include "../person.hpp"
#include "../dto.hpp"

using namespace matador;

JsonMapperTestUnit::JsonMapperTestUnit()
  : unit_test("json_mapper", "json test")
{
  add_test("fields", [this] { test_fields(); }, "test mapping object with builtin fields");
  add_test("array_builtins", [this] { test_array_of_builtins(); }, "test mapping array of builtin fields");
  add_test("array_objects", [this] { test_array_of_objects(); }, "test mapping array of objects");
  add_test("nested_object", [this] { test_nested_object(); }, "test mapping nested object");
  add_test("nested_array_of_object", [this] { test_nested_array_of_object(); }, "test mapping nested array of objects");
  add_test("complex", [this] { test_complex(); }, "test mapping complex object");
  add_test("failures", [this] { test_failure(); }, "test mapping failures");
  add_test("false_types", [this] { test_false_types(); }, "test mapping with false types");
  add_test("special_chars", [this] { test_special_chars(); }, "test mapping special characters");
  add_test("json_to_string", [this] { test_json_to_string(); }, "test json to string");
}

void JsonMapperTestUnit::test_fields()
{
  json_mapper mapper;

  auto p = mapper.to_object<dto>(R"(  {
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
  json_mapper mapper;

  auto p = mapper.to_object<dto>(R"(  {
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
  json_mapper sub_mapper;

  auto subs = sub_mapper.to_objects<bounding_box>(
    R"( [ { "length": 200, "width":  300, "height":   100 }, { "length": 900, "width":  800, "height":   700 } ] )");

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
  json_mapper mapper;

  auto p = mapper.to_object<dto>(R"(  {
"dimension": { "length": 200, "width":  300, "height":   100 }
} )");

  UNIT_EXPECT_EQUAL(200, p.dimension.length);
  UNIT_EXPECT_EQUAL(300, p.dimension.width);
  UNIT_EXPECT_EQUAL(100, p.dimension.height);
}

void JsonMapperTestUnit::test_nested_array_of_object()
{
  json_mapper mapper;

  auto p = mapper.to_object<dto>(R"(  {
"dimensions": [{ "length": 200, "width":  300, "height":   100 }, { "length": 900, "width":  800, "height":   700 }]
} )");

  UNIT_EXPECT_EQUAL(2UL, p.dimensions.size());
}

void JsonMapperTestUnit::test_complex()
{
  json_mapper mapper;

  auto p = mapper.to_object<dto>(R"(  {
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
  json_mapper mapper;

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(    )"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(    --)"), json_exception, "root must be object '{}'");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  key)"), json_exception, "expected string opening quotes");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key)"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key  )"), json_exception, "invalid stream");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key" ; )"), json_exception, "character isn't colon");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key": "value" )"), json_exception, "not a valid object closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key": "value" ]  )"), json_exception, "not a valid object closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  {  "key": "value" } hhh  )"), json_exception, "no characters are allowed after closed root node");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  8)"), json_exception, "not a valid array closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  8 )"), json_exception, "not a valid array closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  8 fff)"), json_exception, "not a valid array closing bracket");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  12345678901234567890123456789 ]   )"), json_exception, "errno integer error");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  1.79769e+309 ]   )"), json_exception, "errno double error");

  UNIT_ASSERT_EXCEPTION(mapper.to_objects<person>(R"(  [  1.79769e+308ddd ]   )"), json_exception, "invalid json character");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": t)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": tg)"), json_exception, "invalid character for bool value string");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": f)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": fu)"), json_exception, "invalid character for bool value string");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": w)"), json_exception, "unknown json type");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": n)"), json_exception, "unexpected end of string");

  UNIT_ASSERT_EXCEPTION(mapper.to_object<person>(R"(  { "key": na)"), json_exception, "invalid null character");
}

void JsonMapperTestUnit::test_false_types()
{
  json_mapper mapper;

  // check false types
  auto p = mapper.to_object<dto>(R"(  {     "id":  9, "name": true,
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
  json_mapper pmapper;

  person pp(pmapper.to_object<person>(R"({ "name": "\r\ferik\tder\nwikinger\b\u0085"})"));

  UNIT_EXPECT_EQUAL("\r\ferik\tder\nwikinger\b\\u0085", pp.name());
}

void JsonMapperTestUnit::test_json_to_string()
{
  json js = json::object();

  js["name"] = "Herb";
  js["numbers"] = json::array();
  js["numbers"].push_back(1);
  js["numbers"].push_back(2);
  js["numbers"].push_back(3);
  js["question"] = true;
  js["pi"] = 3.1415;
  js["obj"] = {{ "type", "car" },{ "id", 17 }};

  json_mapper mapper;

  auto result = mapper.to_string(js);

  std::string expected_result { R"({"name": "Herb", "numbers": [1, 2, 3], "obj": {"id": 17, "type": "car"}, "pi": 3.141500, "question": true})" };

  UNIT_ASSERT_EQUAL(expected_result, result);

  json js2 = mapper.to_json(result);

  UNIT_ASSERT_EQUAL(js, js2);

  js = json::array();
  json j = {{"id", 17}, {"type", "car"}};
  js.push_back(j);
  j = {{"name", "Herb"}, {"numbers", {1, 2, 3}}};
  js.push_back(j);
  js.push_back(true);
  js.push_back(3.1415);

  result = mapper.to_string(js);

  expected_result = R"([{"id": 17, "type": "car"}, {"name": "Herb", "numbers": [1, 2, 3]}, true, 3.141500])";

  UNIT_ASSERT_EQUAL(expected_result, result);

  js2 = mapper.to_json(result.c_str());

  UNIT_ASSERT_EQUAL(js, js2);
}
