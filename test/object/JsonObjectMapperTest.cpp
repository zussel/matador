//
// Created by sascha on 15.01.20.
//

#include "JsonObjectMapperTest.hpp"
#include "../person.hpp"

#include "matador/object/json_object_mapper.hpp"

using namespace matador;

JsonObjectMapperTest::JsonObjectMapperTest()
  : unit_test("object_json", "Object Json Mapper Test")
{
  add_test("simple", std::bind(&JsonObjectMapperTest::test_simple, this), "test simple json object mapper");
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
