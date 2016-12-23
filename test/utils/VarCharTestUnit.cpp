#include "VarCharTestUnit.hpp"

#include "oos/utils/varchar.hpp"

#include <iostream>

using oos::varchar;
using std::cout;
using std::endl;

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
  
  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str8.size(), 0, "size of varchar must be zero");
  
  str8 = "Hallo";
  
  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str8.size(), 5, "size of varchar must be zero");
  
  str8 += std::string("Welt");

  UNIT_ASSERT_EQUAL((int)str8.capacity(), 8, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str8.size(), 8, "size of varchar must be zero");
}

void VarCharTestUnit::test_copy()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2(str8);

  UNIT_ASSERT_EQUAL(str8_2, "Hallo", "expected string must be 'Hallo'");
}

void VarCharTestUnit::test_assign()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2 = str8;

  UNIT_ASSERT_EQUAL(str8_2, "Hallo", "expected string must be 'Hallo'");

  str8 = "Baba";

  UNIT_ASSERT_EQUAL(str8, "Baba", "expected string must be 'Baba'");
}

void VarCharTestUnit::test_init()
{
  varchar<16> str("hallo");

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str.size(), 5, "size of varchar must be zero");
  UNIT_ASSERT_TRUE(str.valid(), "varchar must be valid");
  UNIT_ASSERT_EQUAL(str, "hallo", "size of varchar must be zero");
}

void VarCharTestUnit::test_modify()
{
  varchar<16> str("hallo");

  str += " welt";

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str.size(), 10, "size of varchar must be zero");
  UNIT_ASSERT_TRUE(str.valid(), "varchar must be valid");
  UNIT_ASSERT_EQUAL(str, "hallo welt", "size of varchar must be zero");

  str = "hallo";

  varchar<6> echo(" echo");

  str += echo;

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str.size(), 10, "size of varchar must be zero");
  UNIT_ASSERT_TRUE(str.valid(), "varchar must be valid");
  UNIT_ASSERT_EQUAL(str, "hallo echo", "size of varchar must be zero");

  echo = "1234567";

  UNIT_ASSERT_EQUAL((int)echo.capacity(), 6, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)echo.size(), 7, "size of varchar must be zero");
  UNIT_ASSERT_FALSE(echo.valid(), "varchar must be invalid");
  UNIT_ASSERT_EQUAL(echo, "1234567", "size of varchar must be zero");

  echo.trim();

  UNIT_ASSERT_EQUAL((int)echo.size(), 6, "size of varchar must be zero");
  UNIT_ASSERT_TRUE(echo.valid(), "varchar must be valid");
  UNIT_ASSERT_EQUAL(echo, "123456", "size of varchar must be zero");
}

void VarCharTestUnit::test_access()
{
  varchar<16> str("hello");

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str.size(), 5, "size of varchar must be zero");
  UNIT_ASSERT_TRUE(str.valid(), "varchar must be valid");
  UNIT_ASSERT_EQUAL(str, "hello", "unexpected value");
  UNIT_ASSERT_EQUAL(str.str(), "hello", "unexpected value");
  UNIT_ASSERT_EQUAL(str.c_str(), "hello", "unexpected value");
  UNIT_ASSERT_EQUAL(str.data(), "hello", "unexpected value");
}
