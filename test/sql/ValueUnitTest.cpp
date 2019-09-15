//
// Created by sascha on 03.01.17.
//

#include "ValueUnitTest.hpp"
#include "../datatypes.hpp"

ValueUnitTest::ValueUnitTest() : unit_test("value", "value test unit")
{
  add_test("serialize", std::bind(&ValueUnitTest::test_serialize, this), "test serialize values");
}

void ValueUnitTest::test_serialize()
{
  datatypes dt;

  //value_serializer vs();

}
