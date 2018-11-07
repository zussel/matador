//
// Created by sascha on 6/9/17.
//

#include <matador/object/object_view.hpp>

#include "../Item.hpp"
#include "../entities.hpp"
#include "../Blog.hpp"

#include "RelationTestUnit.hpp"

RelationTestUnit::RelationTestUnit()
  : unit_test("relation", "Relation Test Unit")
{
  add_test("has_one", std::bind(&RelationTestUnit::test_has_one, this), "test has one relation");

  add_test("insert_belongs_to_one", std::bind(&RelationTestUnit::test_insert_belongs_to_one, this), "test belongs to one relation");
  add_test("belongs_to_one", std::bind(&RelationTestUnit::test_belongs_to_one, this), "test belongs to one relation");
  add_test("remove_belongs_to_one", std::bind(&RelationTestUnit::test_remove_belongs_to_one, this), "test remove belongs to one relation");

  add_test("insert_belongs_to_many", std::bind(&RelationTestUnit::test_insert_belongs_to_many, this), "test insert belongs to many relation");
  add_test("belongs_to_many", std::bind(&RelationTestUnit::test_belongs_to_many, this), "test belongs to many relation");
  add_test("remove_belongs_to_many", std::bind(&RelationTestUnit::test_remove_belongs_to_many, this), "test remove belongs to many relation");
  add_test("belongs_to_many_first_belongs_to", std::bind(&RelationTestUnit::test_belongs_to_many_first_belongs_to, this), "test belongs to many relation where belongs to is registered first");

  add_test("insert_has_many_vector", std::bind(&RelationTestUnit::test_insert_has_many_vector, this), "test insert has many vector relation");
  add_test("has_many_vector", std::bind(&RelationTestUnit::test_has_many_vector, this), "test has many vector relation");
//  add_test("remove_has_many_vector", std::bind(&RelationTestUnit::test_remove_has_many_vector, this), "test remove has many vector relation");

  add_test("has_many_list", std::bind(&RelationTestUnit::test_has_many_list, this), "test has many list relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");

  add_test("has_many_to_many", std::bind(&RelationTestUnit::test_has_many_to_many, this), "test has many to many relation");
  add_test("remove_has_many_to_many", std::bind(&RelationTestUnit::test_remove_has_many_to_many, this), "test remove has many to many relation");

//  add_test("remove_has_many_object", std::bind(&RelationTestUnit::test_remove_has_many_object, this), "test remove has many object relation");
//  add_test("remove_has_many_builtin", std::bind(&RelationTestUnit::test_remove_has_many_builtin, this), "test remove has many builtin relation");

  add_test("blog_single", std::bind(&RelationTestUnit::test_blog_single_post, this), "test blog single post relations");
  add_test("blog_multi", std::bind(&RelationTestUnit::test_blog_multi_posts, this), "test blog multiple posts relations");
}

void RelationTestUnit::test_has_one()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<master>("master");

  UNIT_ASSERT_EQUAL(2UL, store.size(), "must be two nodes");

  auto node = store.find("master");

  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "child", "endpoint field name must be 'child'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HS_ONE");

  node = store.find<child>();

  UNIT_ASSERT_TRUE(node->endpoints_empty(), "endpoints must be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 0UL, "endpoints must be zero");

  auto child1 = store.insert(new child("child 1"));

  UNIT_ASSERT_TRUE(child1.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(child1.reference_count(), 0UL, "ref count must be zero");

  auto m1 = store.insert(new master("m1", child1));

  UNIT_ASSERT_TRUE(m1.id() > 0UL, "id must be greater zero");

  UNIT_ASSERT_NOT_NULL(m1->children.get(), "id must not be null");
  UNIT_ASSERT_EQUAL(child1.reference_count(), 1UL, "ref count must be one");

  m1->children = nullptr;

  UNIT_ASSERT_NULL(m1->children.get(), "id must be null");
  UNIT_ASSERT_EQUAL(child1.reference_count(), 0UL, "ref count must be zero");

  store.remove(m1);

  UNIT_ASSERT_EQUAL(store.find<master>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 1UL, "size must be zero");

  store.remove(child1);

  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL, "size must be zero");

  auto m1_ptr = std::make_unique<master>("m1");
  auto child1_ptr = std::make_unique<child>("child 1");

  m1_ptr->children = child1_ptr.release();

  m1 = store.insert(m1_ptr.release());
  child1 = m1->children;

  UNIT_ASSERT_TRUE(m1.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_TRUE(child1.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(child1.reference_count(), 1UL, "ref count must be one");

  store.remove(m1);

  UNIT_ASSERT_EQUAL(store.find<master>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL, "size must be zero");
}

void RelationTestUnit::test_insert_belongs_to_one()
{
  matador::object_store store;

  store.attach<person>("person", matador::object_store::abstract);
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address", "endpoint field name must be 'address'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen", "endpoint field name must be 'citizen'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  // create a citizen and set its address
  // than insert that combination into store
  // both citizen and address must be inserted
  // and the relation must be resolved
  // address' citizen must be set
  auto george_ptr = std::make_unique<citizen>("george");
  george_ptr->address_ = new address("foreststreet", "foresting");

  auto george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must not be null");

  auto home = george->address_;

  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must not be null");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL, "size must be zero");

  george_ptr = std::make_unique<citizen>("george");
  home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(home.reference_count(), 0UL, "ref count must be one");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must not be null");

  george_ptr->address_ = home;

  george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must not be null");
  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must not be null");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL, "size must be zero");
}

void RelationTestUnit::test_belongs_to_one()
{
  matador::object_store store;

  store.attach<person>("person", matador::object_store::abstract);
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address", "endpoint field name must be 'address'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen", "endpoint field name must be 'citizen'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must be null");

  george->address_ = home;

  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must not be null");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must not be null");

  george->address_ = nullptr;

  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must be null");

  home->citizen_ = george;

  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must be null");
}

void RelationTestUnit::test_remove_belongs_to_one()
{
  matador::object_store store;

  store.attach<person>("person", matador::object_store::abstract);
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address", "endpoint field name must be 'address'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen", "endpoint field name must be 'citizen'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must be null");

  george->address_ = home;

  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must not be null");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must not be null");
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL, "ref count must be one");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_FALSE(home.valid(), "home must be null");

  george = store.insert(new citizen("george"));

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");

  home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_FALSE(home->citizen_.valid(), "id must be null");

  home->citizen_ = george;

  UNIT_ASSERT_TRUE(george->address_.valid(), "id must not be null");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_TRUE(home->citizen_.valid(), "id must not be null");
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL, "ref count must be one");

  store.remove(home);

  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_TRUE(george.valid(), "george must be valid");
  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_FALSE(george->address_.valid(), "id must be null");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_FALSE(george.valid(), "home must be null");
}

void RelationTestUnit::test_insert_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department", "endpoint field name must be 'department'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee", "endpoint field name must be 'employee'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto george_ptr = std::make_unique<employee>("george");
  george_ptr->dep(new department("insurance"));

  auto george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(george->dep().get(), "id must not be null");

  auto insurance = george->dep();

  UNIT_ASSERT_EQUAL(insurance.id(), 0UL, "id must be zero");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "size must be one");

  insurance = store.insert(insurance);

  UNIT_ASSERT_GREATER(insurance.id(), 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "size must be one");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 1UL, "size must be one");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "size must be zero");

  store.remove(insurance);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 0UL, "size must be zero");

  george_ptr = std::make_unique<employee>("george");
  insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "size must be zero");

  george_ptr->dep(insurance);

  george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_NOT_NULL(george->dep().get(), "id must not be null");
  UNIT_ASSERT_TRUE(insurance.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "size must be one");

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 1UL, "size must be one");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "size must be zero");

  store.remove(insurance);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 0UL, "size must be zero");
}

void RelationTestUnit::test_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department", "endpoint field name must be 'department'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee", "endpoint field name must be 'employee'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  jane->dep(nullptr);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  insurance->employees.push_back(jane);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  insurance->employees.clear();

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  insurance->employees.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");
}

void RelationTestUnit::test_remove_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department", "endpoint field name must be 'department'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee", "endpoint field name must be 'employee'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new employee("jane"));
  auto george = store.insert(new employee("george"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "reference of insurance count must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "reference of jane count must be zero");
  UNIT_ASSERT_NULL(george->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(george.reference_count(), 0UL, "reference of george count must be zero");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "reference of insurance count must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "reference of jane count must be zero");

  store.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");

  george->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(george->dep(), insurance, "objects must be equal");

  store.remove(insurance);

  UNIT_ASSERT_NULL(george->dep().get(), "object must be null");
}

void RelationTestUnit::test_belongs_to_many_first_belongs_to()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<employee, person>("employee");
  store.attach<department>("department");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department", "endpoint field name must be 'department'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee", "endpoint field name must be 'employee'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  jane->dep(nullptr);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  insurance->employees.push_back(jane);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  insurance->employees.clear();

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");

  insurance->employees.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");
}

void RelationTestUnit::test_insert_has_many_vector()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("children_vector");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children", "endpoint field name must be 'children'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id", "endpoint field name must be 'child_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("vector_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "vector_id", "endpoint field name must be 'vector_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  auto tim_ptr = std::make_unique<child>("tim");
  auto group_ptr = std::make_unique<children_vector>("group");

  UNIT_ASSERT_TRUE(group_ptr->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group_ptr->children.size(), 0UL, "vector size must be zero");

  group_ptr->children.push_back(tim_ptr.release());

  UNIT_ASSERT_FALSE(group_ptr->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group_ptr->children.size(), 1UL, "vector size must be one");

  auto group = store.insert(group_ptr.release());

  UNIT_ASSERT_GREATER(group.id(), 0UL, "id must be greater zero");
  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");

  auto tim = group->children.front();

  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_GREATER(tim.id(), 0UL, "object id must be valid");

  group->children.clear();

  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL, "ref count must be one");
  UNIT_ASSERT_GREATER(tim.id(), 0UL, "object id must be valid");

  try {
    store.remove(tim);
  } catch (matador::object_exception &ex) {
    UNIT_FAIL("couldn't remove tim " + std::string(ex.what()));
  }

  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_TRUE(group->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "vector size must be zero");

  store.remove(group);

  UNIT_ASSERT_EQUAL(store.find<children_vector>()->size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(store.find("children")->size(), 0UL, "size must be zero");
}

void RelationTestUnit::test_has_many_vector()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("children_vector");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children", "endpoint field name must be 'children'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id", "endpoint field name must be 'child_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("vector_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "vector_id", "endpoint field name must be 'vector_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_vector("group"));

  UNIT_ASSERT_TRUE(group->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "vector size must be zero");

  group->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim", "name must be 'tim'");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL, "ref count must be one");

  group->children.remove(tim);

  UNIT_ASSERT_TRUE(group->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_GREATER(tim.id(), 0UL, "object id must be valid");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL, "ref count must be zero");

  group->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim", "name must be 'tim'");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL, "ref count must be one");

  group->children.clear();

  UNIT_ASSERT_TRUE(group->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_GREATER(tim.id(), 0UL, "object id must be valid");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL, "ref count must be zero");
}

void RelationTestUnit::test_remove_has_many_vector()
{

}

void RelationTestUnit::test_has_many_list()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_list>("children_list");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("children_list");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children", "endpoint field name must be 'children'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

//  auto foreign_endpoint = endpoint->second->foreign_endpoint.lock();
//
//  UNIT_ASSERT_NOT_NULL(foreign_endpoint.get(), "foreign endpoint must be valid");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->field, "list_id", "endpoint field name must be 'list_id'");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id", "endpoint field name must be 'child_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("list_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id", "endpoint field name must be 'list_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_list("group"));

  UNIT_ASSERT_TRUE(group->children.empty(), "list must be empty");
  UNIT_ASSERT_EQUAL(group.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "list size must be zero");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL, "ref count must be zero");

  group->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim", "name must be 'tim'");
}

void RelationTestUnit::test_has_many_builtin()
{
  using many_vector_ints = many_builtins<int, std::vector>;
//  using many_vector_strings = many_builtins<std::string, std::vector>;

  matador::object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  UNIT_ASSERT_EQUAL(2UL, store.size(), "must be two nodes");

  auto node = store.find("many_vector_ints");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->find_endpoint("elements");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "elements", "endpoint field name must be 'elements'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("elements");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->find_endpoint("list_id");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id", "endpoint field name must be 'list_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  endpoint = node->find_endpoint("value");

  UNIT_ASSERT_TRUE(endpoint == node->endpoint_end(), "must not find endpoint 'value'");

  auto many_ints = store.insert(new many_vector_ints);

  UNIT_ASSERT_TRUE(many_ints->elements.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 0UL, "vector size must be zero");

  many_ints->elements.push_back(7);

  UNIT_ASSERT_FALSE(many_ints->elements.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(many_ints->elements.front(), 7, "values must be equal");
}

void RelationTestUnit::test_has_many_to_many()
{
//  std::cout << "\n";
  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, store.size(), "must be four nodes");

  auto node = store.find("student");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course", "endpoint field name must be 'student_course'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("student_course");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("course_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "endpoint must be found");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "course_id", "endpoint field name must be 'course_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("student_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "endpoint must be found");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_id", "endpoint field name must be 'student_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<course>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course", "endpoint field name must be 'student_course'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new student("jane"));
  auto tom = store.insert(new student("tom"));
  auto art = store.insert(new course("art"));

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");

  jane->courses.push_back(art);

  UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");

  art->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art.reference_count(), 2UL, "ref count must be two");
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be two");
  UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");

  art->students.remove(tom);

  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), jane, "objects must be same");

  jane->courses.clear();

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");
}

void RelationTestUnit::test_remove_has_many_object()
{

}

void RelationTestUnit::test_remove_has_many_builtin()
{

}

void RelationTestUnit::test_remove_has_many_to_many()
{
//  std::cout << "\n";
  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  auto jane = store.insert(new student("jane"));
  auto tom = store.insert(new student("tom"));
  auto art = store.insert(new course("art"));

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL, "ref count must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL, "ref count must be zero");

  jane->courses.push_back(art); // jane (value) must be push_back to course art (owner) students!!

  UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL, "ref count must be one");

  art->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");
  UNIT_ASSERT_EQUAL(art.reference_count(), 2UL, "reference count must be 2");

  store.remove(tom);

  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), jane, "objects must be same");
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL, "reference count must be 1");
}

void RelationTestUnit::test_blog_single_post()
{
  matador::object_store store;

  store.attach<blog_detail::person>("person", matador::object_store::abstract);
  store.attach<author, blog_detail::person>("author");
  store.attach<category>("category");
  store.attach<comment>("comment");
  store.attach<post>("post");

  auto me = store.insert(new author("sascha", matador::date(29, 4, 1972)));
  auto main = store.insert(new category("Main", "Main category"));

  auto post1 = store.insert(new post("My first post", me, main, "My first post content"));

  using t_post_view = matador::object_view<post>;
  t_post_view posts(store);

  UNIT_ASSERT_EQUAL(store.find("post_category")->size(), 1UL, "size must be one");

  auto hi = post1->categories.begin().holder_item();

  UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL, "ref count must be two");

  UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be three");

  UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL, "ref count must be one");

  UNIT_ASSERT_EQUAL(main.reference_count(), 1UL, "ref count must be one");
  UNIT_ASSERT_EQUAL(me->posts.size(), 1UL, "size must be one");
  UNIT_ASSERT_EQUAL(main->posts.size(), 1UL, "size must be one");

  auto comment_one = store.insert(new comment("alfons@mail.fr", "cool stuff"));

  UNIT_ASSERT_GREATER(comment_one.id(), 0UL, "id must be valid");
  UNIT_ASSERT_FALSE(comment_one->blog_post.valid(), "post must be invalid");
  UNIT_ASSERT_EQUAL(post1->comments.size(), 0UL, "size must be zero");

  post1->comments.push_back(comment_one);

  UNIT_ASSERT_TRUE(comment_one->blog_post.valid(), "post must be valid");
  UNIT_ASSERT_EQUAL(post1->comments.size(), 1UL, "size must be one");

  store.remove(post1);

  UNIT_ASSERT_EQUAL(posts.size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(me->posts.size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(me.reference_count(), 0UL, "ref count must be 0");
  UNIT_ASSERT_EQUAL(main->posts.size(), 0UL, "size must be zero");
  UNIT_ASSERT_EQUAL(main.reference_count(), 0UL, "ref count must be 0");
}

void RelationTestUnit::test_blog_multi_posts()
{
  matador::object_store store;

  store.attach<blog_detail::person>("person", matador::object_store::abstract);
  store.attach<author, blog_detail::person>("author");
  store.attach<category>("category");
  store.attach<comment>("comment");
  store.attach<post>("post");

  auto me = store.insert(new author("sascha", matador::date(29, 4, 1972)));
  auto main = store.insert(new category("Main", "Main category"));

  auto post1 = store.insert(new post("My first post", me, main, "My first post content"));
  auto post2 = store.insert(new post("My second post", me, main, "My second post content"));
  auto post3 = store.insert(new post("My third post", me, main, "My third post content"));
  auto post4 = store.insert(new post("My fourth post", me, main, "My fourth post content"));

  using t_post_view = matador::object_view<post>;
  t_post_view posts(store);

  UNIT_ASSERT_EQUAL(posts.size(), 4UL, "size must be three");

  UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL, "ref count must be 1");
  UNIT_ASSERT_EQUAL(post2.reference_count(), 2UL, "ref count must be 1");
  UNIT_ASSERT_EQUAL(post3.reference_count(), 2UL, "ref count must be 1");
  UNIT_ASSERT_EQUAL(post4.reference_count(), 2UL, "ref count must be 1");

  UNIT_ASSERT_EQUAL(main->posts.size(), 4UL, "size must be four");
  UNIT_ASSERT_EQUAL(main.reference_count(), 4UL, "ref count must be 4");
  UNIT_ASSERT_EQUAL(me->posts.size(), 4UL, "size must be four");
  UNIT_ASSERT_EQUAL(me.reference_count(), 4UL, "ref count must be 4");

  store.remove(post3);

  UNIT_ASSERT_EQUAL(main->posts.size(), 3UL, "size must be three");
  UNIT_ASSERT_EQUAL(main.reference_count(), 3UL, "ref count must be 3");
  UNIT_ASSERT_EQUAL(me->posts.size(), 3UL, "size must be three");
  UNIT_ASSERT_EQUAL(me.reference_count(), 3UL, "ref count must be 3");
}
