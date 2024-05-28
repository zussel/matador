#include <catch2/catch_test_macros.hpp>

#include "matador/object/prototype_node.hpp"
#include "matador/object/observer_list_copy_creator.hpp"

#include <memory>

using namespace matador;

template < class Type >
struct typed_observer_one : public typed_object_store_observer<Type>
{
  std::vector<std::string> nodes;
  std::string name;
  typed_observer_one()
  : name("one_" + std::string(typeid(Type).name()))
  {}

  template < class Other >
  explicit typed_observer_one(const typed_observer_one<Other> &)
  : name("one_" + std::string(typeid(Other).name()) + "_copied")
  {}


  void on_attach(prototype_node &node, Type &) override { nodes.emplace_back(node.type()); }
  void on_detach(prototype_node &node, Type &) override { nodes.emplace_back(node.type()); }
  void on_insert(object_proxy &proxy) override { nodes.emplace_back(std::string("insert ") + proxy.node()->type()); }
  void on_update(object_proxy &proxy) override { nodes.emplace_back(std::string("update ") + proxy.node()->type()); }
  void on_delete(object_proxy &proxy) override { nodes.emplace_back(std::string("delete ") + proxy.node()->type()); }
};

template < class Type >
struct typed_observer_two : public typed_object_store_observer<Type>
{
  std::vector<std::string> nodes;
  std::string name;
  typed_observer_two()
  : name("two_" + std::string(typeid(Type).name()))
  {}

  template < class Other >
  explicit typed_observer_two(const typed_observer_two<Other> &)
  : name("two_" + std::string(typeid(Other).name()) + "_copied")
  {}

  void on_attach(prototype_node &node, Type &) override { nodes.emplace_back(node.type()); }
  void on_detach(prototype_node &node, Type &) override { nodes.emplace_back(node.type()); }
  void on_insert(object_proxy &proxy) override { nodes.emplace_back(std::string("insert ") + proxy.node()->type()); }
  void on_update(object_proxy &proxy) override { nodes.emplace_back(std::string("update ") + proxy.node()->type()); }
  void on_delete(object_proxy &proxy) override { nodes.emplace_back(std::string("delete ") + proxy.node()->type()); }
};

struct person {};
struct student {};

TEST_CASE("Copy typed object store observer", "[observer][object_store]") {
  std::vector<std::unique_ptr<typed_object_store_observer<person>>> observers;
  observers.push_back(std::make_unique<typed_observer_one<person>>());
  observers.push_back(std::make_unique<typed_observer_two<person>>());

  auto copied_observes = observer_list_copy_creator<person, student, typed_observer_one, typed_observer_two>::copy_create(observers);

  REQUIRE(copied_observes.size() == 2);
  const std::string student_name = typeid(person).name();
  auto *one_obs = dynamic_cast<typed_observer_one<student>*>(copied_observes[0].get());
  REQUIRE(one_obs != nullptr);
  REQUIRE(one_obs->name == "one_" + student_name + "_copied");
  auto *two_obs = dynamic_cast<typed_observer_two<student>*>(copied_observes[1].get());
  REQUIRE(two_obs != nullptr);
  REQUIRE(two_obs->name == "two_" + student_name + "_copied");
}