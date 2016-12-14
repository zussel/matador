#include "VarCharTestUnit.hpp"

#include "oos/utils/varchar.hpp"

#include <iostream>

using oos::varchar;
using std::cout;
using std::endl;

VarCharTestUnit::VarCharTestUnit()
  : unit_test("varchar", "varchar test unit")
{
  add_test("create", std::bind(&VarCharTestUnit::create_varchar, this), "create varchar");
  add_test("copy", std::bind(&VarCharTestUnit::copy_varchar, this), "copy varchar");
  add_test("assign", std::bind(&VarCharTestUnit::assign_varchar, this), "assign varchar");
  add_test("init", std::bind(&VarCharTestUnit::init_varchar, this), "init varchar");
}

void VarCharTestUnit::create_varchar()
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

void VarCharTestUnit::copy_varchar()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2(str8);

  UNIT_ASSERT_EQUAL(str8_2, "Hallo", "expected string must be 'Hallo'");
}

void VarCharTestUnit::assign_varchar()
{
  varchar<8> str8("Hallo");

  varchar<8> str8_2 = str8;

  UNIT_ASSERT_EQUAL(str8_2, "Hallo", "expected string must be 'Hallo'");

  str8 = "Baba";

  UNIT_ASSERT_EQUAL(str8, "Baba", "expected string must be 'Baba'");
}

void VarCharTestUnit::init_varchar()
{
  varchar<16> str("hallo");

  UNIT_ASSERT_EQUAL((int)str.capacity(), 16, "invalid capacity of varchar");
  UNIT_ASSERT_EQUAL((int)str.size(), 5, "size of varchar must be zero");
  UNIT_ASSERT_EQUAL(str, "hallo", "size of varchar must be zero");
}
