//
// Created by sascha on 6/9/17.
//

#include <matador/object/object_view.hpp>

#include "../datatypes.hpp"
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

  add_test("has_many_list", std::bind(&RelationTestUnit::test_has_many_list, this), "test has many list relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");

  add_test("has_many_to_many", std::bind(&RelationTestUnit::test_has_many_to_many, this), "test has many to many relation");
  add_test("remove_has_many_to_many", std::bind(&RelationTestUnit::test_remove_has_many_to_many, this), "test remove has many to many relation");

  add_test("blog_single", std::bind(&RelationTestUnit::test_blog_single_post, this), "test blog single post relations");
  add_test("blog_multi", std::bind(&RelationTestUnit::test_blog_multi_posts, this), "test blog multiple posts relations");
}

void RelationTestUnit::test_has_one()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<master>("master");

  UNIT_ASSERT_EQUAL(2UL, store.size());

  auto node = store.find("master");

  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "child");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  node = store.find<child>();

  UNIT_ASSERT_TRUE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 0UL);

  auto child1 = store.insert(new child("child 1"));

  UNIT_ASSERT_TRUE(child1.id() > 0UL);
  UNIT_ASSERT_EQUAL(child1.reference_count(), 0UL);

  auto m1 = store.insert(new master("m1", child1));

  UNIT_ASSERT_TRUE(m1.id() > 0UL);

  UNIT_ASSERT_NOT_NULL(m1->children.get());
  UNIT_ASSERT_EQUAL(child1.reference_count(), 1UL);

  m1.modify()->children = nullptr;

  UNIT_ASSERT_NULL(m1->children.get());
  UNIT_ASSERT_EQUAL(child1.reference_count(), 0UL);

  store.remove(m1);

  UNIT_ASSERT_EQUAL(store.find<master>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 1UL);

  store.remove(child1);

  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL);

  auto m1_ptr = std::make_unique<master>("m1");
  auto child1_ptr = std::make_unique<child>("child 1");

  m1_ptr->children = child1_ptr.release();

  m1 = store.insert(m1_ptr.release());
  child1 = m1->children;

  UNIT_ASSERT_TRUE(m1.id() > 0UL);
  UNIT_ASSERT_TRUE(child1.id() > 0UL);
  UNIT_ASSERT_EQUAL(child1.reference_count(), 1UL);

  store.remove(m1);

  UNIT_ASSERT_EQUAL(store.find<master>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL);
}

void RelationTestUnit::test_insert_belongs_to_one()
{
  matador::object_store store;

  store.attach_abstract<person>("person");
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  // create a citizen and set its address
  // than insert that combination into store
  // both citizen and address must be inserted
  // and the relation must be resolved
  // address' citizen must be set
  auto george_ptr = std::make_unique<citizen>("george");
  george_ptr->address_ = new address("foreststreet", "foresting");

  auto george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(george->address_.get());

  auto home = george->address_;

  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(home->citizen_.get());

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL);

  george_ptr = std::make_unique<citizen>("george");
  home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_EQUAL(home.reference_count(), 0UL);
  UNIT_ASSERT_NULL(home->citizen_.get());

  george_ptr->address_ = home;

  george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(george->address_.get());
  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(home->citizen_.get());

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL);
}

void RelationTestUnit::test_belongs_to_one()
{
  matador::object_store store;

  store.attach_abstract<person>("person");
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_NULL(george->address_.get());
  UNIT_ASSERT_NULL(home->citizen_.get());

  george.modify()->address_ = home;

  UNIT_ASSERT_NOT_NULL(george->address_.get());
  UNIT_ASSERT_NOT_NULL(home->citizen_.get());

  george.modify()->address_ = nullptr;

  UNIT_ASSERT_NULL(george->address_.get());
  UNIT_ASSERT_NULL(home->citizen_.get());

  home.modify()->citizen_ = george;

  UNIT_ASSERT_NOT_NULL(george->address_.get());
  UNIT_ASSERT_NOT_NULL(home->citizen_.get());
}

void RelationTestUnit::test_remove_belongs_to_one()
{
  matador::object_store store;

  store.attach_abstract<person>("person");
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_NULL(george->address_.get());
  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_NULL(home->citizen_.get());

  george.modify()->address_ = home;

  UNIT_ASSERT_NOT_NULL(george->address_.get());
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(home->citizen_.get());
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL);

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL);
  UNIT_ASSERT_FALSE(home.valid());

  george = store.insert(new citizen("george"));

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_NULL(george->address_.get());

  home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(home.id() > 0UL);
  UNIT_ASSERT_FALSE(home->citizen_.valid());

  home.modify()->citizen_ = george;

  UNIT_ASSERT_TRUE(george->address_.valid());
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_TRUE(home->citizen_.valid());
  UNIT_ASSERT_EQUAL(home.reference_count(), 1UL);

  store.remove(home);

  UNIT_ASSERT_EQUAL(store.find<address>()->size(), 0UL);
  UNIT_ASSERT_TRUE(george.valid());
  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_FALSE(george->address_.valid());

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<citizen>()->size(), 0UL);
  UNIT_ASSERT_FALSE(george.valid());
}

void RelationTestUnit::test_insert_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  auto george_ptr = std::make_unique<employee>("george");
  auto dep = new department("insurance");
  george_ptr->dep(dep);

  auto george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(george->dep().get());

  auto insurance = george->dep();

  UNIT_ASSERT_EQUAL(insurance.id(), 0UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);

  insurance = store.insert(insurance);

  UNIT_ASSERT_GREATER(insurance.id(), 0UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 1UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);

  store.remove(insurance);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 0UL);

  george_ptr = std::make_unique<employee>("george");
  insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance.id() > 0UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);

  george_ptr->dep(insurance);

  george = store.insert(george_ptr.release());

  UNIT_ASSERT_TRUE(george.id() > 0UL);
  UNIT_ASSERT_EQUAL(george.reference_count(), 1UL);
  UNIT_ASSERT_NOT_NULL(george->dep().get());
  UNIT_ASSERT_TRUE(insurance.id() > 0UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);

  store.remove(george);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 1UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);

  store.remove(insurance);

  UNIT_ASSERT_EQUAL(store.find<employee>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find<department>()->size(), 0UL);
}

void RelationTestUnit::test_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  jane.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  jane.modify()->dep(nullptr);

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  insurance.modify()->employees.push_back(jane);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  insurance.modify()->employees.clear();

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  jane.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  insurance.modify()->employees.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
}

void RelationTestUnit::test_remove_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  auto jane = store.insert(new employee("jane"));
  auto george = store.insert(new employee("george"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
  UNIT_ASSERT_NULL(george->dep().get());
  UNIT_ASSERT_EQUAL(george.reference_count(), 0UL);

  jane.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  store.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);

  george.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(george->dep(), insurance);

  store.remove(insurance);

  UNIT_ASSERT_NULL(george->dep().get());
}

void RelationTestUnit::test_belongs_to_many_first_belongs_to()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<employee, person>("employee");
  store.attach<department>("department");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  jane.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  jane.modify()->dep(nullptr);

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  insurance.modify()->employees.push_back(jane);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  insurance.modify()->employees.clear();

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

  jane.modify()->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->dep(), insurance);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

  insurance.modify()->employees.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty());
  UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
  UNIT_ASSERT_NULL(jane->dep().get());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
}

void RelationTestUnit::test_insert_has_many_vector()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("children_vector");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL);

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  endpoint = node->find_endpoint("vector_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "vector_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  auto tim_ptr = std::make_unique<child>("tim");
  auto group_ptr = std::make_unique<children_vector>("group");

  UNIT_ASSERT_TRUE(group_ptr->children.empty());
  UNIT_ASSERT_EQUAL(group_ptr->children.size(), 0UL);

  group_ptr->children.push_back(tim_ptr.release());

  UNIT_ASSERT_FALSE(group_ptr->children.empty());
  UNIT_ASSERT_EQUAL(group_ptr->children.size(), 1UL);

  auto group = store.insert(group_ptr.release());

  UNIT_ASSERT_GREATER(group.id(), 0UL);
  UNIT_ASSERT_FALSE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL);

  auto tim = group->children.front();

  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL);
  UNIT_ASSERT_GREATER(tim.id(), 0UL);

  group.modify()->children.clear();

  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL);
  UNIT_ASSERT_GREATER(tim.id(), 0UL);

  try {
    store.remove(tim);
  } catch (matador::object_exception &ex) {
    UNIT_FAIL("couldn't remove tim " + std::string(ex.what()));
  }

  UNIT_ASSERT_EQUAL(store.find<child>()->size(), 0UL);
  UNIT_ASSERT_TRUE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL);

  store.remove(group);

  UNIT_ASSERT_EQUAL(store.find<children_vector>()->size(), 0UL);
  UNIT_ASSERT_EQUAL(store.find("children")->size(), 0UL);
}

void RelationTestUnit::test_has_many_vector()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("children_vector");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL);

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  endpoint = node->find_endpoint("vector_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "vector_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_vector("group"));

  UNIT_ASSERT_TRUE(group->children.empty());
  UNIT_ASSERT_EQUAL(group.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL);

  group.modify()->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL);
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL);

  group.modify()->children.remove(tim);

  UNIT_ASSERT_TRUE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL);
  UNIT_ASSERT_GREATER(tim.id(), 0UL);
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL);

  group.modify()->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL);
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim");
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL);

  group.modify()->children.clear();

  UNIT_ASSERT_TRUE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL);
  UNIT_ASSERT_GREATER(tim.id(), 0UL);
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL);
}

void RelationTestUnit::test_has_many_list()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_list>("children_list");

  UNIT_ASSERT_EQUAL(3UL, store.size());

  auto node = store.find("children_list");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

//  auto foreign_endpoint = endpoint->second->foreign_endpoint.lock();
//
//  UNIT_ASSERT_NOT_NULL(foreign_endpoint.get());
//  UNIT_ASSERT_EQUAL(foreign_endpoint->field, "list_id");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL);

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  endpoint = node->find_endpoint("list_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_list("group"));

  UNIT_ASSERT_TRUE(group->children.empty());
  UNIT_ASSERT_EQUAL(group.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL);
  UNIT_ASSERT_EQUAL(tim.reference_count(), 0UL);

  group.modify()->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty());
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL);
  UNIT_ASSERT_EQUAL(tim.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim");
}

void RelationTestUnit::test_has_many_builtin()
{
  using local_many_vector_ints = many_builtins<int, std::vector>;
//  using many_vector_strings = many_builtins<std::string, std::vector>;

  matador::object_store store;

  store.attach<local_many_vector_ints>("local_many_vector_ints");

  UNIT_ASSERT_EQUAL(2UL, store.size());

  auto node = store.find("local_many_vector_ints");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->find_endpoint("elements");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "elements");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  node = store.find("elements");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->find_endpoint("list_id");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE);

  endpoint = node->find_endpoint("value");

  UNIT_ASSERT_TRUE(endpoint == node->endpoint_end());

  auto many_ints = store.insert(new local_many_vector_ints);

  UNIT_ASSERT_TRUE(many_ints->elements.empty());
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 0UL);

  many_ints.modify()->elements.push_back(7);

  UNIT_ASSERT_FALSE(many_ints->elements.empty());
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 1UL);
  UNIT_ASSERT_EQUAL(many_ints->elements.front(), 7);
}

void RelationTestUnit::test_has_many_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, store.size());

  auto node = store.find("student");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  node = store.find("student_course");
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL);

  endpoint = node->find_endpoint("course_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "course_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  endpoint = node->find_endpoint("student_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end());
  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_id");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO);

  node = store.find<course>();
  UNIT_ASSERT_TRUE(node != store.end());
  UNIT_ASSERT_FALSE(node->endpoints_empty());
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL);

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY);

  auto jane = store.insert(new student("jane"));
  auto tom = store.insert(new student("tom"));
  auto art = store.insert(new course("art"));

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL);
  UNIT_ASSERT_TRUE(tom->courses.empty());
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
  UNIT_ASSERT_TRUE(art->students.empty());
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL);

  jane.modify()->courses.push_back(art);

  UNIT_ASSERT_FALSE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.front(), art);
  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.front(), jane);

  art.modify()->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty());
  UNIT_ASSERT_EQUAL(tom.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(tom->courses.front(), art);
  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL);
  UNIT_ASSERT_EQUAL(art->students.back(), tom);

  art.modify()->students.remove(tom);

  UNIT_ASSERT_TRUE(tom->courses.empty());
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.back(), jane);

  jane.modify()->courses.clear();

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL);
  UNIT_ASSERT_TRUE(art->students.empty());
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL);
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

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);
  UNIT_ASSERT_TRUE(tom->courses.empty());
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL);
  UNIT_ASSERT_TRUE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL);
  UNIT_ASSERT_EQUAL(art.reference_count(), 0UL);

  jane.modify()->courses.push_back(art); // jane (value) must be push_back to course art (owner) students!!

  UNIT_ASSERT_FALSE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.front(), art);
  UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);
  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.front(), jane);
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL);

  art.modify()->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty());
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(tom->courses.front(), art);
  UNIT_ASSERT_EQUAL(tom.reference_count(), 1UL);
  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL);
  UNIT_ASSERT_EQUAL(art->students.back(), tom);
  UNIT_ASSERT_EQUAL(art.reference_count(), 2UL);

  store.remove(tom);

  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.back(), jane);
  UNIT_ASSERT_EQUAL(art.reference_count(), 1UL);
}

void RelationTestUnit::test_blog_single_post()
{
  matador::object_store store;

  store.attach_abstract<blog_detail::person>("person");
  store.attach<author, blog_detail::person>("author");
  store.attach<category>("category");
  store.attach<comment>("comment");
  store.attach<post>("post");

  auto me = store.insert(new author("sascha", matador::date(29, 4, 1972)));
  auto main = store.insert(new category("Main", "Main category"));

  auto post1 = store.insert(new post("My first post", me, main, "My first post content"));

  using t_post_view = matador::object_view<post>;
  t_post_view posts(store);

  UNIT_ASSERT_EQUAL(store.find("post_category")->size(), 1UL);

  auto hi = post1->categories.begin().holder_item();

  UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL);

  UNIT_ASSERT_EQUAL(posts.size(), 1UL);

  UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL);

  UNIT_ASSERT_EQUAL(main.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(me->posts.size(), 1UL);
  UNIT_ASSERT_EQUAL(main->posts.size(), 1UL);

  auto comment_one = store.insert(new comment("alfons@mail.fr", "cool stuff"));

  UNIT_ASSERT_GREATER(comment_one.id(), 0UL);
  UNIT_ASSERT_FALSE(comment_one->blog_post.valid());
  UNIT_ASSERT_EQUAL(post1->comments.size(), 0UL);

  post1.modify()->comments.push_back(comment_one);

  UNIT_ASSERT_TRUE(comment_one->blog_post.valid());
  UNIT_ASSERT_EQUAL(post1->comments.size(), 1UL);

  store.remove(post1);

  UNIT_ASSERT_EQUAL(posts.size(), 0UL);
  UNIT_ASSERT_EQUAL(me->posts.size(), 0UL);
  UNIT_ASSERT_EQUAL(me.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(main->posts.size(), 0UL);
  UNIT_ASSERT_EQUAL(main.reference_count(), 0UL);
}

void RelationTestUnit::test_blog_multi_posts()
{
  matador::object_store store;

  store.attach_abstract<blog_detail::person>("person");
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

  UNIT_ASSERT_EQUAL(posts.size(), 4UL);

  UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(post2.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(post3.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(post4.reference_count(), 2UL);

  UNIT_ASSERT_EQUAL(main->posts.size(), 4UL);
  UNIT_ASSERT_EQUAL(main.reference_count(), 4UL);
  UNIT_ASSERT_EQUAL(me->posts.size(), 4UL);
  UNIT_ASSERT_EQUAL(me.reference_count(), 4UL);

  store.remove(post3);

  UNIT_ASSERT_EQUAL(main->posts.size(), 3UL);
  UNIT_ASSERT_EQUAL(main.reference_count(), 3UL);
  UNIT_ASSERT_EQUAL(me->posts.size(), 3UL);
  UNIT_ASSERT_EQUAL(me.reference_count(), 3UL);
}
