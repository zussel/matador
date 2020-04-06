//
// Created by sascha on 15.01.20.
//

#include "JsonObjectMapperTest.hpp"
#include "../person.hpp"

#include "matador/object/json_object_mapper.hpp"
#include "../entities.hpp"
#include "../has_many_list.hpp"

using namespace matador;

JsonObjectMapperTest::JsonObjectMapperTest()
  : unit_test("json_object", "Json Object Mapper Test")
{
  add_test("simple", std::bind(&JsonObjectMapperTest::test_simple, this), "test simple json object mapper");
  add_test("derived", std::bind(&JsonObjectMapperTest::test_derived, this), "test derived json object mapper");
  add_test("has_many", std::bind(&JsonObjectMapperTest::test_has_many, this), "test has many json object mapper");
}

void JsonObjectMapperTest::test_simple()
{
  json_object_mapper<person> mapper;

  auto p = mapper.object_from_string(R"(  { "id":  5, "name": "george", "height": 185 } )");

  UNIT_EXPECT_EQUAL(5UL, p.id());
  UNIT_EXPECT_EQUAL("george", p.name());
  UNIT_EXPECT_EQUAL(185U, p.height());
}

void JsonObjectMapperTest::test_derived()
{
  json_object_mapper<citizen> mapper;

  auto p = mapper.object_from_string(R"(  { "id":  5, "name": "george", "height": 185, "birthdate": "2001-11-27", "address": { "id": 4, "street": "east-street", "city": "east-city", "citizen": 5 } } )");

  date birthday(27, 11, 2001);
  UNIT_EXPECT_EQUAL(5UL, p.id());
  UNIT_EXPECT_EQUAL("george", p.name());
  UNIT_EXPECT_EQUAL(185U, p.height());
  UNIT_EXPECT_EQUAL(birthday, p.birthdate());

  UNIT_ASSERT_NOT_NULL(p.address_.get());

  object_ptr<address> addr = p.address_;

  UNIT_EXPECT_EQUAL(4UL, addr->id);
  UNIT_EXPECT_EQUAL("east-street", addr->street);
  UNIT_EXPECT_EQUAL("east-city", addr->city);
}

void JsonObjectMapperTest::test_has_many()
{
  json_object_mapper<hasmanylist::owner> mapper;

  auto p = mapper.object_from_string(R"(  {
"id":  5,
"name": "george",
"owner_item": [{"id": 13, "name": "strawberry"}, {"id": 17, "name": "banana"}   ]
} )");

  UNIT_EXPECT_EQUAL(5UL, p.id);
  UNIT_EXPECT_EQUAL("george", p.name);
  UNIT_ASSERT_EQUAL(2UL, p.items.size());

  const auto &i = p.items.front();
  UNIT_ASSERT_EQUAL(13UL, i->id);
  UNIT_ASSERT_EQUAL("strawberry", i->name);
}
