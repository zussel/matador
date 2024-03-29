#include "JsonOrmTest.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/utils/date.hpp"

#include "matador/object/json_object_mapper.hpp"
#include "matador/object/container.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

JsonOrmTest::JsonOrmTest(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_json_orm", prefix + " json orm test unit")
  , dns_(std::move(dns))
{
  add_test("insert", [this] { test_insert_from_json(); }, "insert from json string test");
}

using namespace matador;

namespace ormjson {
struct person
{
  unsigned long id{};   // primary key
  std::string name;
  date birthday;
  container<std::string> colors {255};

  person() = default;
  person(long i, std::string n)
  : id(i), name(std::move(n))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "birthday", birthday);
    matador::access::has_many(op, "person_color",  // relation table name
                              colors,          // class member
                              "person_id",     // left column in relation table
                              "color",         // right column in relation table
                              matador::cascade_type::ALL); // cascade type
  }
};
}

void JsonOrmTest::test_insert_from_json()
{
  // prepare the persistence layer
  persistence p(dns_);
  p.attach<ormjson::person>("person");

  // create tables
  p.create();

  json_object_mapper mapper;

  std::string data = R"({
    "name": "george",
    "birthday": "2001-11-15",
    "person_color": ["green", "blue"]
  })";

  std::string expected_result = R"({
  "id": 1,
  "name": "george",
  "birthday": "2001-11-15",
  "person_color": ["green","blue"]
}

)";

  auto ptr = mapper.to_object<ormjson::person>(data);

  session s(p);

  auto optr = s.insert(ptr.release());
  s.flush();

  auto body = mapper.to_string(optr, json_format::pretty);

  UNIT_ASSERT_EQUAL(expected_result, body);

  p.drop();
}
