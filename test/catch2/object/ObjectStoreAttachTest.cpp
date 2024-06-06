#include <catch2/catch_test_macros.hpp>

#include "matador/object/object_store.hpp"

namespace matador::test {

struct simple {
  unsigned long id{};
  std::string name;
  double value{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "value", value);
  }
};

struct hammer
{
  unsigned long id{};
  std::string name;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
  }
};

struct toolbox
{
  unsigned long id{};
  std::string name;
  object_ptr<hammer> hammer_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "hammer", hammer_);
  }
};

struct profile;

struct user
{
  unsigned long id{};
  std::string name;
  object_ptr<profile> profile_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_one(op, "id", profile_);
  }
};

struct profile
{
  unsigned long id{};
  std::string name;
  object_ptr<user> user_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "user_id", user_);
  }
};

struct department;

struct employee
{
  unsigned long id{};
  std::string name;
  object_ptr<department> department_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "department_id", department_);
  }
};

struct department
{
  unsigned long id{};
  std::string name;
  container<employee> employees;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many(op, employees, "id");
  }
};

struct course;

struct student
{
  unsigned long id{};
  std::string name;
  container<course> courses;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "student_course", courses, "student_id", "course_id");
  }
};

struct course
{
  unsigned long id{};
  std::string name;
  container<student> students;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "student_course", students);
  }
};

}

using namespace matador;

TEST_CASE("Attach type without relation", "[object_store][attach][relation]") {
  object_store store;

  const auto it = store.attach<test::simple>("simple");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("simple"));
  REQUIRE(it->endpoints_empty());
}

TEST_CASE("Attach type with belongs to relation", "[object_store][attach][relation]") {
  object_store store;

  auto it = store.attach<test::hammer>("hammer");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("hammer"));
  REQUIRE(it->endpoints_empty());

  it = store.attach<test::toolbox>("toolbox");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("toolbox"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  const auto ep = it->endpoint_begin();

  REQUIRE(ep->first == std::type_index(typeid(test::hammer)));

  const auto rel = ep->second;

  REQUIRE(rel->type == detail::basic_relation_endpoint::BELONGS_TO);
  REQUIRE(rel->field == "hammer");
}

TEST_CASE("Attach types with belongs to and has one relations", "[object_store][attach][relation]") {
  object_store store;

  auto it = store.attach<test::user>("user");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("user"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  it = store.attach<test::profile>("profile");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("profile"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);
}

TEST_CASE("Attach types with belongs to and has many relations", "[object_store][attach][relation]") {
  object_store store;

  auto it = store.attach<test::employee>("employee");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("employee"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  it = store.attach<test::department>("department");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("department"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);
}

TEST_CASE("Attach types with has many to many relations", "[object_store][attach][relation]") {
  object_store store;

  auto it = store.attach<test::student>("student");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("student"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  auto ep = it->find_endpoint("student_course");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "student_course");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::HAS_MANY);

  auto student_relation_endpoint = ep->second;

  auto course_relation_endpoint = student_relation_endpoint->foreign_endpoint.lock();
  REQUIRE(!course_relation_endpoint);

  it = store.attach<test::course>("course");

  course_relation_endpoint = student_relation_endpoint->foreign_endpoint.lock();
  REQUIRE(course_relation_endpoint);

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("course"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  ep = it->find_endpoint("student_course");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "student_course");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::HAS_MANY);
  auto fep = ep->second->foreign_endpoint;
}