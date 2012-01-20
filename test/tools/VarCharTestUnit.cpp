#include "VarCharTestUnit.hpp"

#include "tools/varchar.hpp"

#include <iostream>

using oos::varchar;
using oos::tvarchar;
using std::cout;
using std::endl;

VarCharTestUnit::VarCharTestUnit()
  : unit_test("varchar test unit")
{
  add_test("create", std::tr1::bind(&VarCharTestUnit::create_varchar, this), "create varchar");
  add_test("copy", std::tr1::bind(&VarCharTestUnit::copy_varchar, this), "copy varchar");
  add_test("assign", std::tr1::bind(&VarCharTestUnit::assign_varchar, this), "assign varchar");
}

VarCharTestUnit::~VarCharTestUnit()
{}

void VarCharTestUnit::create_varchar()
{
  cout << endl;
  tvarchar<8> str8;
  
  cout << "varchar<" << str8.capacity() << "/" << str8.size() << "> = [" << str8 << "]\n";
  
  str8 = "Hallo";
  
  cout << "varchar<" << str8.capacity() << "/" << str8.size() << "> = [" << str8 << "]\n";
  
  str8 += std::string("Welt");

  cout << "varchar<" << str8.capacity() << "/" << str8.size() << "> = [" << str8 << "]\n";
}

void VarCharTestUnit::copy_varchar()
{
}

void VarCharTestUnit::assign_varchar()
{
}
