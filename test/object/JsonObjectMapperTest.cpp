//
// Created by sascha on 15.01.20.
//

#include "JsonObjectMapperTest.hpp"
#include "../person.hpp"

#include "matador/object/json_object_mapper.hpp"
#include "../entities.hpp"

using namespace matador;

JsonObjectMapperTest::JsonObjectMapperTest()
  : unit_test("object_json", "Object Json Mapper Test")
{
  add_test("simple", std::bind(&JsonObjectMapperTest::test_simple, this), "test simple json object mapper");
  add_test("derived", std::bind(&JsonObjectMapperTest::test_derived, this), "test derived json object mapper");
}

void JsonObjectMapperTest::test_simple()
{
  json_object_mapper<person> mapper;

  std::unique_ptr<person> p(mapper.from_string(R"(  { "id":  5, "name": "george", "height": 185 } )"));

  UNIT_ASSERT_NOT_NULL(p.get());
  UNIT_EXPECT_EQUAL(5UL, p->id());
  UNIT_EXPECT_EQUAL("george", p->name());
  UNIT_EXPECT_EQUAL(185U, p->height());
}

void JsonObjectMapperTest::test_derived()
{
  //std::cout << "\n";
  json_object_mapper<citizen> mapper;

  std::unique_ptr<citizen> p(mapper.from_string(R"(  { "id":  5, "name": "george", "height": 185, "birthdate": "2001-11-27", "address": { "id": 4, "street": "east-street", "city": "east-city", "citizen": 5 } } )"));

  date birthday(27, 11, 2001);
  UNIT_ASSERT_NOT_NULL(p.get());
  UNIT_EXPECT_EQUAL(5UL, p->id());
  UNIT_EXPECT_EQUAL("george", p->name());
  UNIT_EXPECT_EQUAL(185U, p->height());
  UNIT_EXPECT_EQUAL(birthday, p->birthdate());

  UNIT_ASSERT_NOT_NULL(p->address_.get());

  object_ptr<address> addr = p->address_;

  UNIT_EXPECT_EQUAL(4UL, addr->id);
  UNIT_EXPECT_EQUAL("east-street", addr->street);
  UNIT_EXPECT_EQUAL("east-city", addr->city);
}
