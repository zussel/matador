//
// Created by sascha on 27.01.20.
//

#include "JsonIdentifierMapperTest.hpp"

#include "matador/utils/varchar.hpp"

using namespace matador;

JsonIdentifierMapperTest::JsonIdentifierMapperTest()
: unit_test("json_identifier", "json identifier mapper test")
{
  add_test("integer", std::bind(&JsonIdentifierMapperTest::test_integer_identifier, this), "test integer identifier types");
  add_test("varchar", std::bind(&JsonIdentifierMapperTest::test_varchar_identifier, this), "test varchar identifier types");
}

void JsonIdentifierMapperTest::test_integer_identifier()
{
  json_identifier_mapper id_mapper;

  check_integer_identifier<char>(5, id_mapper);
  check_integer_identifier<short>(5, id_mapper);
  check_integer_identifier<int>(5, id_mapper);
  check_integer_identifier<long>(5, id_mapper);
  check_integer_identifier<long long>(5, id_mapper);
  check_integer_identifier<unsigned char>(5, id_mapper);
  check_integer_identifier<unsigned short>(5, id_mapper);
  check_integer_identifier<unsigned int>(5, id_mapper);
  check_integer_identifier<unsigned long>(5, id_mapper);
  check_integer_identifier<unsigned long long>(5, id_mapper);
}

void JsonIdentifierMapperTest::test_varchar_identifier()
{
  json_identifier_mapper id_mapper;

  matador::json val("hallo");
  matador::identifier<varchar<255>> str_id;
  id_mapper.set_identifier_value(str_id, val);
  UNIT_EXPECT_EQUAL("hallo", str_id.value());

  matador::identifier<varchar<255, char*>> char_id;
  id_mapper.set_identifier_value(char_id, val);
  UNIT_ASSERT_EQUAL("hallo", char_id.value());
}