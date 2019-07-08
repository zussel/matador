#include "VarCharTestUnit.hpp"

#include "matador/utils/varchar.hpp"

#include <iostream>

using matador::varchar;

VarCharTestUnit::VarCharTestUnit()
  : unit_test("varchar", "varchar test unit")
{
  add_test("create", std::bind(&VarCharTestUnit::test_create, this), "create varchar");
  add_test("copy", std::bind(&VarCharTestUnit::test_copy, this), "copy varchar");
  add_test("assign", std::bind(&VarCharTestUnit::test_assign, this), "assign varchar");
  add_test("init", std::bind(&VarCharTestUnit::test_init, this), "init varchar");
  add_test("modify", std::bind(&VarCharTestUnit::test_modify, this), "modify varchar");
  add_test("access", std::bind(&VarCharTestUnit::test_access, this), "access varchar");
}

void VarCharTestUnit::test_create()
{
  varchar<8> str8;
  
  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8);
  UNIT_ASSERT_EQUAL((int)str8.size(), 0);
  
  str8 = "Hallo";
  
  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8);
  UNIT_ASSERT_EQUAL((int)str8.size(), 5);
  
  str8 += std::string("Welt");

  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8);
  UNIT_ASSERT_EQUAL((int)str8.size(), 8);
}

void VarCharTestUnit::test_copy()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2(str8);

  UNIT_ASSERT_EQUAL(str8_2, "Hallo");
}

void VarCharTestUnit::test_assign()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2 = str8;

  UNIT_ASSERT_EQUAL(str8_2, "Hallo");

  str8 = "Baba";

  UNIT_ASSERT_EQUAL(str8, "Baba");
}

void VarCharTestUnit::test_init()
{
  varchar<16> str("hallo");

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16);
  UNIT_ASSERT_EQUAL((int)str.size(), 5);
  UNIT_ASSERT_TRUE(str.valid());
  UNIT_ASSERT_EQUAL(str, "hallo");
}

void VarCharTestUnit::test_modify()
{
  varchar<16> str("hallo");

  str += " welt";

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16);
  UNIT_ASSERT_EQUAL((int)str.size(), 10);
  UNIT_ASSERT_TRUE(str.valid());
  UNIT_ASSERT_EQUAL(str, "hallo welt");

  str = "hallo";

  varchar<6> echo(" echo");

  str += echo;

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16);
  UNIT_ASSERT_EQUAL((int)str.size(), 10);
  UNIT_ASSERT_TRUE(str.valid());
  UNIT_ASSERT_EQUAL(str, "hallo echo");

  echo = "1234567";

  UNIT_ASSERT_EQUAL((int)echo.capacity(), 6);
  UNIT_ASSERT_EQUAL((int)echo.size(), 7);
  UNIT_ASSERT_FALSE(echo.valid());
  UNIT_ASSERT_EQUAL(echo, "1234567");

  echo.trim();

  UNIT_ASSERT_EQUAL((int)echo.size(), 6);
  UNIT_ASSERT_TRUE(echo.valid());
  UNIT_ASSERT_EQUAL(echo, "123456");
}

void VarCharTestUnit::test_access()
{
  varchar<16> str("hello");

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16);
  UNIT_ASSERT_EQUAL((int)str.size(), 5);
  UNIT_ASSERT_TRUE(str.valid());
  UNIT_ASSERT_EQUAL(str, "hello");
  UNIT_ASSERT_EQUAL(str.str(), "hello");
  UNIT_ASSERT_EQUAL(str.c_str(), "hello");
  UNIT_ASSERT_EQUAL(str.data(), "hello");
}
