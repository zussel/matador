//
// Created by sascha on 8/11/15.
//

#include "PrimaryKeyUnitTest.hpp"

#include "object/identifier.hpp"

#include <memory>

PrimaryKeyUnitTest::PrimaryKeyUnitTest()
  : unit_test("pk", "Primary Key Unit Test")
{
  add_test("create", std::bind(&PrimaryKeyUnitTest::test_create, this), "test create");
}

PrimaryKeyUnitTest::~PrimaryKeyUnitTest()
{ }

void PrimaryKeyUnitTest::initialize()
{

}

void PrimaryKeyUnitTest::finalize()
{

}

void PrimaryKeyUnitTest::test_create()
{
  long id57(57);
  std::unique_ptr<oos::basic_identifier> pkb(new oos::identifier<long>(id57));

  pkb->is_valid();

  long result = pkb->id<long>();

  UNIT_ASSERT_EQUAL(id57, result, "values are must be equal");
}
