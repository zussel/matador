#include <catch2/catch_test_macros.hpp>

#include "matador/object/object_store.hpp"
#include "matador/object/object_exception.hpp"

#include "../../datatypes.hpp"

#include "ObjectStoreAttachTestModels.hpp"

using namespace matador;

TEST_CASE("Validates empty object store", "[object_store]") {
  object_store store;

  REQUIRE(store.empty());
}

TEST_CASE("Find prototype nodes", "[object_store]") {
  object_store store;
  store.attach<datatypes>("item");

  REQUIRE(store.size() == 1);

  prototype_iterator i = store.find<datatypes>();

  REQUIRE(i != store.end());
}

TEST_CASE("Validates size of prototype node", "[object_store]") {
  object_store store;

  auto i = store.begin();

  REQUIRE(i->size() == 0);

  store.attach<datatypes>("item");

  i = store.find<datatypes>();

  REQUIRE(i->size() == 0);

  store.insert<datatypes>();

  REQUIRE(i->size() == 1);
}

TEST_CASE("Validates parent of prototype node", "[object_store]") {
  object_store store;

  auto root = store.begin();

  REQUIRE(root->size() == 0);

  store.attach<datatypes>("item");

  auto i = store.find<datatypes>();

  REQUIRE(!root->is_child_of(*i));

  auto sub = store.attach<ItemA, datatypes>("sub");

  REQUIRE(sub->is_child_of(*i));
}

TEST_CASE("Iterate prototype node", "[object_store]") {
  object_store store;
  store.attach<datatypes>("item");
  auto item_a_iterator = store.attach<ItemA, datatypes>("item_a");

  auto i = store.end();

  --i;

  REQUIRE(i == item_a_iterator);
  REQUIRE(--i == store.begin());
}

TEST_CASE("Validate one prototype node in object store", "[object_store]") {
  object_store store;
  store.attach<datatypes>("item");

  auto o = store.create<datatypes>();

  REQUIRE(o);

  o.reset();

  store.detach("item");

  REQUIRE_THROWS_AS(store.create<datatypes>(), object_exception);
}

TEST_CASE("Validates prototype node hierarchy", "[object_store]") {
  object_store store;
  store.attach<datatypes>("ITEM");
  store.attach<ItemA, datatypes>("ITEM_A");
  store.attach<ItemB, datatypes>("ITEM_B");
  store.attach<ItemC, datatypes>("ITEM_C");

  auto a = store.create<ItemB>();

  REQUIRE(a);

  a.reset();

  store.detach("ITEM_B");

  REQUIRE_THROWS_AS(store.create<ItemB>(), object_exception);

  store.detach("ITEM");

  REQUIRE_THROWS_AS(store.create<datatypes>(), object_exception);
  REQUIRE_THROWS_AS(store.create<ItemA>(), object_exception);
  REQUIRE_THROWS_AS(store.create<ItemC>(), object_exception);
}

TEST_CASE("Traverses a prototype node hierarchy", "[object_store]") {
  object_store store;
  store.attach<datatypes>("ITEM");
  store.attach<ItemA, datatypes>("ITEM_A");
  store.attach<ItemB, datatypes>("ITEM_B");
  store.attach<ItemC, datatypes>("ITEM_C");

  auto first = store.begin();
  auto last = store.end();
  int count(0);

  while (first != last) {
    REQUIRE(count < 4);
    ++first;
    ++count;
  }

  REQUIRE(count == 4);
}

TEST_CASE("Get object description", "[object_store][description]") {
  object_store store;
  store.attach<datatypes>("item");

  auto desc = store.describe("item");

  store.attach<test::hammer>("hammer");
  store.attach<test::toolbox>("toolbox");

  desc = store.describe("toolbox");
}