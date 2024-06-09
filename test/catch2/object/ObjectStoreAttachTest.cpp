#include <catch2/catch_test_macros.hpp>

#include "matador/object/object_store.hpp"

#include "ObjectStoreAttachTestModels.hpp"

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

TEST_CASE("Attach types with has many to many relations (master first)", "[object_store][attach][relation]") {
  object_store store;

  // attach master part of many-to-many relation first
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
  REQUIRE(student_relation_endpoint->foreign_endpoint.expired());
  REQUIRE(!course_relation_endpoint);

  // attach slave part of many-to-many relation
  it = store.attach<test::course>("course");

  // validate that both endpoints point to each other
  // and are not the same
  course_relation_endpoint = student_relation_endpoint->foreign_endpoint.lock();
  REQUIRE(!student_relation_endpoint->foreign_endpoint.expired());
  REQUIRE(course_relation_endpoint);
  REQUIRE(course_relation_endpoint != student_relation_endpoint);

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("course"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  ep = it->find_endpoint("student_course");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "student_course");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::HAS_MANY);

  REQUIRE(ep->second == course_relation_endpoint);
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == student_relation_endpoint);

  // get relation node and validate its endpoints
  it = store.find("student_course");
  REQUIRE(it != store.end());
  REQUIRE(it->endpoints_size() == 2);
  ep = it->find_endpoint("student_id");
  REQUIRE(ep->second->field == "student_id");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::BELONGS_TO);
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == student_relation_endpoint);
  ep = it->find_endpoint("course_id");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "course_id");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::BELONGS_TO);
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == course_relation_endpoint);
}

TEST_CASE("Attach types with has many to many relations (slave first)", "[object_store][attach][relation]") {
  object_store store;

  // attach master part of many-to-many relation first
  auto it = store.attach<test::course>("course");

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("course"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  auto ep = it->find_endpoint("student_course");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "student_course");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::HAS_MANY);

  auto course_relation_endpoint = ep->second;
  auto student_relation_endpoint = course_relation_endpoint->foreign_endpoint.lock();

  REQUIRE(course_relation_endpoint->foreign_endpoint.expired());
  REQUIRE(!student_relation_endpoint);

  // attach slave part of many-to-many relation
  it = store.attach<test::student>("student");

  // validate that both endpoints point to each other
  // and are not the same
  student_relation_endpoint = course_relation_endpoint->foreign_endpoint.lock();
  REQUIRE(!course_relation_endpoint->foreign_endpoint.expired());
  REQUIRE(student_relation_endpoint);
  REQUIRE(student_relation_endpoint != course_relation_endpoint);

  REQUIRE(it != store.end());
  REQUIRE(it->type() == std::string("student"));
  REQUIRE(!it->endpoints_empty());
  REQUIRE(it->endpoints_size() == 1);

  ep = it->find_endpoint("student_course");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "student_course");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::HAS_MANY);

  REQUIRE(ep->second == student_relation_endpoint);
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == course_relation_endpoint);

  // get relation node and validate its endpoints
  it = store.find("student_course");
  REQUIRE(it != store.end());
  REQUIRE(it->endpoints_size() == 2);
  ep = it->find_endpoint("student_id");
  REQUIRE(ep->second->field == "student_id");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::BELONGS_TO);
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == student_relation_endpoint);
  ep = it->find_endpoint("course_id");
  REQUIRE(ep != it->endpoint_end());
  REQUIRE(ep->second->field == "course_id");
  REQUIRE(ep->second->type == detail::basic_relation_endpoint::BELONGS_TO);
  REQUIRE(!ep->second->foreign_endpoint.expired());
  REQUIRE(ep->second->foreign_endpoint.lock() == course_relation_endpoint);
}