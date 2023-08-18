#include "PrimaryKeyUnitTest.hpp"

#include <memory>

PrimaryKeyUnitTest::PrimaryKeyUnitTest()
  : unit_test("pk", "Primary Key Unit Test")
{
  add_test("create", [this] { test_create(); }, "test create");
  add_test("share", [this] { test_share(); }, "test share");
}

void PrimaryKeyUnitTest::test_create()
{
//  long id57(57);
//  std::unique_ptr<matador::basic_identifier> pkb(new matador::identifier<long>(id57));
//
//  pkb->is_valid();

//  long result = pkb->id<long>();
//
//  UNIT_ASSERT_EQUAL(id57, result);
}

void PrimaryKeyUnitTest::test_share()
{
//  std::string id("max@mustermann.de");
//  std::string gollum("gollum@mittelerde.to");
//  matador::identifier<std::string> email(id);
//
//  UNIT_ASSERT_EQUAL(id, email.value());
//
//  matador::identifier<std::string> shared_email;
//
//  UNIT_ASSERT_TRUE(shared_email.value().empty());
//
////  email.share_with(shared_email);
//
////  UNIT_ASSERT_EQUAL(id, shared_email.value());
//
//  email.value(gollum);
//
//  UNIT_ASSERT_EQUAL(gollum, email.value());
//  UNIT_ASSERT_EQUAL(gollum, shared_email.value());
}
