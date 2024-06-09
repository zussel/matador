#include <catch2/catch_test_macros.hpp>

#include "matador/utils/access.hpp"

#include "matador/object/object_store.hpp"

namespace matador::test {

struct child
{
  unsigned long id{};
  std::string name;

  child() = default;
  explicit child(std::string n) : name(std::move(n)) {}
  ~child() = default;

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name);
  }
};

struct master
{
  unsigned long id{};
  std::string name;
  matador::object_ptr<child> children{};

  master() = default;
  explicit master(std::string n) : name(std::move(n)) {}
  master(std::string n, const matador::object_ptr<child> &c) : name(std::move(n)), children(c) {}
  ~master() = default;

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name);
    field::has_one(op, "child", children, matador::cascade_type::ALL);
  }
};

struct department;

struct employee
{
  unsigned long id{};
  std::string name;
  matador::object_ptr<department> dep;

  employee() = default;
  explicit employee(std::string name) : name(std::move(name)) {}

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name);
    field::belongs_to(op, "department"    , dep, matador::cascade_type::NONE);
    //                     name of table,   object     , cascade
  }
};

struct department
{
  unsigned long id{};
  std::string name;
  matador::container<employee> employees;

  department() = default;
  explicit department(std::string n)
  : name(std::move(n))
  {}

  ~department() = default;

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::has_many(op, "employees", employees, "department", matador::cascade_type::NONE);
    //                                name of table, container,  name of member
    //                                to serialize
  }
};

}

using namespace matador;

TEST_CASE("Test has_one relation", "[has_one][relation]") {
  object_store store;

  store.attach<test::child>("child");
  store.attach<test::master>("master");

  REQUIRE(store.size() == 2);

  auto node = store.find("master");

  REQUIRE(!node->endpoints_empty());
  REQUIRE(node->endpoints_size() ==  1);

  auto endpoint = node->endpoint_begin();

  REQUIRE(endpoint->second->field == "child");
  REQUIRE(endpoint->second->type == matador::detail::basic_relation_endpoint::HAS_ONE);

  node = store.find<test::child>();

  REQUIRE(node->endpoints_empty());
  REQUIRE(node->endpoints_size() == 0);

  auto child1 = store.insert<test::child>("child 1");

  REQUIRE(child1.id() > 0);
  REQUIRE(child1.reference_count() == 0);

  auto m1 = store.insert<test::master>("m1", child1);

  REQUIRE(m1.id() > 0);

  REQUIRE(m1->children.get() != nullptr);
  REQUIRE(child1.reference_count() == 1);

  m1.modify()->children = nullptr;

  REQUIRE(m1->children.get() == nullptr);
  REQUIRE(child1.reference_count() == 0);

  store.remove(m1);

  REQUIRE(store.find<test::master>()->size() == 0);
  REQUIRE(store.find<test::child>()->size() == 1);

  store.remove(child1);

  REQUIRE(store.find<test::child>()->size() == 0);

  auto m1_ptr = std::make_unique<test::master>("m1");
  auto child1_ptr = std::make_unique<test::child>("child 1");

  m1_ptr->children = child1_ptr.release();

  m1 = store.insert(m1_ptr.release());
  child1 = m1->children;

  REQUIRE(m1.id() > 0);
  REQUIRE(child1.id() > 0);
  REQUIRE(child1.reference_count() == 1);

  store.remove(m1);

  REQUIRE(store.find<test::master>()->size() == 0);
  REQUIRE(store.find<test::child>()->size() == 0);
}

TEST_CASE("Test insert belongs to many", "[relation][belongs_to][has_many]") {
  matador::object_store store;

  store.attach<test::department>("department");
  store.attach<test::employee>("employee");

  REQUIRE(store.size() == 2);

  auto node = store.find("employee");
  REQUIRE(node != store.end());
  REQUIRE(!node->endpoints_empty());
  REQUIRE(node->endpoints_size() == 1);

  auto endpoint = node->endpoint_begin();

  REQUIRE(endpoint->second->field == "department");
  REQUIRE(endpoint->second->is_belongs_to());

  node = store.find<test::department>();
  REQUIRE(node != store.end());
  REQUIRE(!node->endpoints_empty());
  REQUIRE(node->endpoints_size() == 1);

  endpoint = node->endpoint_begin();

  REQUIRE(endpoint->second->field == "employee");
  REQUIRE(endpoint->second->is_has_many());

  auto george_ptr = std::make_unique<test::employee>("george");
  auto dep = new test::department("insurance");
  george_ptr->dep = dep;

  auto george = store.insert<test::employee>(george_ptr.release());

  REQUIRE(george.id() > 0UL);
  REQUIRE(george.reference_count() == 1);
  REQUIRE(george->dep.get() != nullptr);

  auto insurance = george->dep;

  REQUIRE(insurance.id() == 0);
  REQUIRE(insurance.reference_count() == 1);
  REQUIRE(insurance->employees.size() == 1);

  insurance = store.insert(insurance);

  REQUIRE(insurance.id() > 0);
  REQUIRE(insurance.reference_count() == 1);
  REQUIRE(insurance->employees.size() == 1);
  REQUIRE(george.reference_count() == 1);

  store.remove(george);

  REQUIRE(store.find<test::employee>()->size() == 0);
  REQUIRE(store.find<test::department>()->size() == 0);
  REQUIRE(insurance.reference_count() == 0);
  REQUIRE(insurance->employees.empty());

  store.remove(insurance);

  REQUIRE(store.find<test::employee>()->size() == 0);
  REQUIRE(store.find<test::department>()->size() == 0);

  george_ptr = std::make_unique<test::employee>("george");
  insurance = store.insert(new test::department("insurance"));

  REQUIRE(insurance.id() > 0UL);
  REQUIRE(insurance.reference_count() == 0);
  REQUIRE(insurance->employees.empty());

  george_ptr->dep = insurance;

  george = store.insert(george_ptr.release());

  REQUIRE(george.id() > 0UL);
  REQUIRE(george.reference_count() == 1);
  REQUIRE(george->dep.get() != nullptr);
  REQUIRE(insurance.id() > 0UL);
  REQUIRE(insurance.reference_count() == 1);
  REQUIRE(!insurance->employees.empty());

  store.remove(george);

  REQUIRE(store.find<test::employee>()->size() == 0);
  REQUIRE(store.find<test::department>()->size() == 1);
  REQUIRE(insurance.reference_count() == 0);
  REQUIRE(insurance->employees.empty());

  store.remove(insurance);

  REQUIRE(store.find<test::employee>()->size() == 0);
  REQUIRE(store.find<test::department>()->size() == 0);
}