//
// Created by sascha on 8/11/15.
//

#include "PrimaryKeyUnitTest.hpp"

#include "matador/utils/identifier.hpp"

#include <memory>

PrimaryKeyUnitTest::PrimaryKeyUnitTest()
  : unit_test("pk", "Primary Key Unit Test")
{
  add_test("create", std::bind(&PrimaryKeyUnitTest::test_create, this), "test create");
  add_test("share", std::bind(&PrimaryKeyUnitTest::test_share, this), "test share");
}

void PrimaryKeyUnitTest::test_create()
{
  long id57(57);
  std::unique_ptr<matador::basic_identifier> pkb(new matador::identifier<long>(id57));

  pkb->is_valid();

//  long result = pkb->id<long>();
//
//  UNIT_ASSERT_EQUAL(id57, result, "values are must be equal");
}

void PrimaryKeyUnitTest::test_share()
{
  std::string id("max@mustermann.de");
  std::string gollum("gollum@mittelerde.to");
  matador::identifier<std::string> email(id);

  UNIT_ASSERT_EQUAL(id, email.value(), "invalid identifier value");

  matador::identifier<std::string> shared_email;

  UNIT_ASSERT_TRUE(shared_email.value().empty(), "identifier must be empty");

//  email.share_with(shared_email);

//  UNIT_ASSERT_EQUAL(id, shared_email.value(), "invalid identifier value");

  email.value(gollum);

  UNIT_ASSERT_EQUAL(gollum, email.value(), "invalid identifier value");
  UNIT_ASSERT_EQUAL(gollum, shared_email.value(), "invalid identifier value");
}
