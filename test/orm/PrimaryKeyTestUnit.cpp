//
// Created by sascha on 27.10.19.
//

#include <matador/utils/identifier.hpp>
#include <matador/orm/persistence.hpp>
#include "PrimaryKeyTestUnit.hpp"

PrimaryKeyTestUnit::PrimaryKeyTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_pk", prefix + " primary key test unit")
  , dns_(std::move(dns))
{
  add_test("int", std::bind(&PrimaryKeyTestUnit::test_integer_pk, this), "test integer primary key");
  add_test("string", std::bind(&PrimaryKeyTestUnit::test_string_pk, this), "test string primary key");
}

void PrimaryKeyTestUnit::test_integer_pk()
{

}

struct user
{
  matador::identifier<std::string> email;
  std::string name;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("email", email);
    s.serialize("name", name, 255);
  }
};

void PrimaryKeyTestUnit::test_string_pk()
{
  matador::persistence p(dns_);

  p.attach<user>("user");

  p.create();


}
