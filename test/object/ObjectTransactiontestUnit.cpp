//
// Created by sascha on 3/9/16.
//

#include "ObjectTransactiontestUnit.hpp"

#include "../Item.hpp"

#include "object/object_store.hpp"
#include "object/transaction.hpp"
#include "object/object_view.hpp"

ObjectTransactiontestUnit::ObjectTransactiontestUnit()
  : unit_test("transaction", "transaction unit test")
{
  add_test("insert", std::bind(&ObjectTransactiontestUnit::test_insert, this), "test transaction insert");
  add_test("insert_rollback", std::bind(&ObjectTransactiontestUnit::test_insert_rollback, this), "test transaction insert rollback");
  add_test("update", std::bind(&ObjectTransactiontestUnit::test_update, this), "test transaction update");
  add_test("update_rollback", std::bind(&ObjectTransactiontestUnit::test_update_rollback, this), "test transaction update rollback");
}


void ObjectTransactiontestUnit::test_insert()
{
  oos::object_store store;
  store.attach<person>("person");

  oos::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr.commit();
  } catch (std::exception &ex) {
    tr.rollback();
  }

  oos::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL, "id must be valid");
  UNIT_ASSERT_EQUAL(p->name(), "Hans", "name must be 'Hans'");
}

void ObjectTransactiontestUnit::test_insert_rollback()
{
  oos::object_store store;
  store.attach<person>("person");

  oos::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr.rollback();
  } catch (std::exception &ex) {
    UNIT_FAIL("shouldn't come here");
  }

  oos::object_view<person> pview(store);

  UNIT_ASSERT_TRUE(pview.empty(), "view must be empty");
}

void ObjectTransactiontestUnit::test_update()
{
  oos::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

  oos::transaction tr(store);

  try {
    tr.begin();


    UNIT_ASSERT_EQUAL(hans->height(), 180U, "height must be valid");
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");
    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr.commit();
  } catch (std::exception &ex) {
    UNIT_FAIL("shouldn't come here");
  }

  UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");
  UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

  oos::object_view<person> pview(store);

  UNIT_ASSERT_FALSE(pview.empty(), "view must not be empty");
}

void ObjectTransactiontestUnit::test_update_rollback()
{
  oos::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

  oos::transaction tr(store);

  try {
    tr.begin();


    UNIT_ASSERT_EQUAL(hans->height(), 180U, "height must be valid");
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");
    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr.rollback();
  } catch (std::exception &ex) {
    UNIT_FAIL("shouldn't come here");
  }

  UNIT_ASSERT_EQUAL(hans->height(), 180U, "height must be valid");
  UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

  oos::object_view<person> pview(store);

  UNIT_ASSERT_FALSE(pview.empty(), "view must not be empty");
}
