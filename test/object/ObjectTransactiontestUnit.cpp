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
  add_test("delete", std::bind(&ObjectTransactiontestUnit::test_delete, this), "test transaction delete");
  add_test("delete_rollback", std::bind(&ObjectTransactiontestUnit::test_delete_rollback, this), "test transaction delete rollback");
  add_test("nested", std::bind(&ObjectTransactiontestUnit::test_nested, this), "test nested transaction");
  add_test("nested_rollback", std::bind(&ObjectTransactiontestUnit::test_nested_rollback, this), "test nested transaction rollback");
  add_test("foreign", std::bind(&ObjectTransactiontestUnit::test_foreign, this), "test transaction foreign object");
  add_test("foreign_rollback", std::bind(&ObjectTransactiontestUnit::test_foreign_rollback, this), "test transaction foreign object rollback");
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
  hans->id(7UL);

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

void ObjectTransactiontestUnit::test_delete()
{
  oos::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

  UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

  oos::transaction tr(store);

  try {
    tr.begin();

    store.remove(hans);

    tr.commit();
  } catch (std::exception &ex) {
    tr.rollback();
  }

  oos::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_EQUAL(p.id(), 0UL, "id must be valid");
  UNIT_ASSERT_TRUE(p.ptr() == nullptr, "object must be nullptr");
}

void ObjectTransactiontestUnit::test_delete_rollback()
{
  oos::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

  UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

  oos::transaction tr(store);

  try {
    tr.begin();

    store.remove(hans);

    tr.rollback();
  } catch (std::exception &ex) {
    tr.rollback();
  }

  oos::object_view<person> pview(store);

  auto p = pview.front();
  UNIT_ASSERT_EQUAL(pview.size(), 1UL, "size must be one");

  UNIT_ASSERT_GREATER(p.id(), 0UL, "id must be valid");
  UNIT_ASSERT_FALSE(p.ptr() == nullptr, "object must be nullptr");
  UNIT_ASSERT_GREATER(p->id(), 0UL, "id must be valid");
  UNIT_ASSERT_EQUAL(p->name(), "Hans", "name must be 'Hans'");
}

void ObjectTransactiontestUnit::test_nested()
{
  oos::object_store store;
  store.attach<person>("person");

  oos::transaction tr1(store);

  try {
    tr1.begin();

    auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr1.commit();

    tr1.begin();

    UNIT_ASSERT_EQUAL(hans->height(), 180U, "height must be valid");
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");

    oos::transaction tr2(store);

    try {
      tr2.begin();

      UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");
      hans->height(159);
      UNIT_ASSERT_EQUAL(hans->height(), 159U, "height must be valid");

      tr2.commit();
    } catch (std::exception &ex) {
      tr2.rollback();
    }

    tr1.commit();
  } catch (std::exception &ex) {
    tr1.rollback();
  }

  oos::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL, "id must be valid");
  UNIT_ASSERT_EQUAL(p->name(), "Hans", "name must be 'Hans'");
}

void ObjectTransactiontestUnit::test_nested_rollback()
{
  oos::object_store store;
  store.attach<person>("person");

  oos::transaction tr1(store);

  try {
    tr1.begin();

    auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr1.commit();

    tr1.begin();

    UNIT_ASSERT_EQUAL(hans->height(), 180U, "height must be valid");
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");

    oos::transaction tr2(store);

    try {
      tr2.begin();

      UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");
      hans->height(159);
      UNIT_ASSERT_EQUAL(hans->height(), 159U, "height must be valid");

      tr2.rollback();

      UNIT_ASSERT_EQUAL(hans->height(), 183U, "height must be valid");

    } catch (std::exception &ex) {
      tr2.rollback();
    }

    tr1.commit();
  } catch (std::exception &ex) {
    tr1.rollback();
  }

  oos::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL, "id must be valid");
  UNIT_ASSERT_EQUAL(p->name(), "Hans", "name must be 'Hans'");
}

void ObjectTransactiontestUnit::test_foreign()
{
  oos::object_store store;
  store.attach<child>("child");
  store.attach<master>("master");

  auto ch1 = store.insert(new child("child 1"));
  auto m1 = store.insert(new master("m1"));
  m1->children = ch1;

  oos::transaction tr(store);

  try {
    tr.begin();

    oos::object_view<master> mview(store);

    m1 = mview.front();

    UNIT_ASSERT_TRUE(m1.ptr() != nullptr, "master must be valid");
    UNIT_ASSERT_TRUE(m1->children.ptr() != nullptr, "child must be valid");
    UNIT_ASSERT_EQUAL(m1->name, "m1", "name must be valid");

    UNIT_ASSERT_TRUE(store.is_removable(m1), "m1 must be removable");

    store.remove(m1);

    UNIT_ASSERT_TRUE(mview.empty(), "view must be empty");

    tr.commit();
  } catch (std::exception &ex) {
    tr.rollback();
  }
}

void ObjectTransactiontestUnit::test_foreign_rollback()
{
  oos::object_store store;
  store.attach<child>("child");
  store.attach<master>("master");

  auto ch1 = store.insert(new child("child 1"));
  auto m1 = store.insert(new master("m1"));
  m1->children = ch1;

  oos::transaction tr(store);

  oos::object_view<master> mview(store);

  m1 = mview.front();

  try {
    tr.begin();

    UNIT_ASSERT_TRUE(m1.ptr() != nullptr, "master must be valid");
    UNIT_ASSERT_TRUE(m1->children.ptr() != nullptr, "child must be valid");
    UNIT_ASSERT_EQUAL(m1->name, "m1", "name must be valid");

    UNIT_ASSERT_TRUE(store.is_removable(m1), "m1 must be removable");

    store.remove(m1);

    UNIT_ASSERT_TRUE(mview.empty(), "view must be empty");

    tr.rollback();
  } catch (std::exception &ex) {
    tr.rollback();
  }

  UNIT_ASSERT_FALSE(mview.empty(), "view must be empty");

}
