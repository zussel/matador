//
// Created by sascha on 3/9/16.
//

#include "ObjectTransactiontestUnit.hpp"

#include "../Item.hpp"
#include "../person.hpp"
#include "../entities.hpp"

#include "matador/object/object_store.hpp"
#include "matador/object/transaction.hpp"
#include "matador/object/object_view.hpp"

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
  matador::object_store store;
  store.attach<person>("person");

  matador::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr.commit();
  } catch (std::exception &) {
    tr.rollback();
  }

  matador::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL);
  UNIT_ASSERT_EQUAL(p->name(), "Hans");
}

void ObjectTransactiontestUnit::test_insert_rollback()
{
  matador::object_store store;
  store.attach<person>("person");

  matador::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr.rollback();
  } catch (std::exception &) {
    UNIT_FAIL("shouldn't come here");
  }

  matador::object_view<person> pview(store);

  UNIT_ASSERT_TRUE(pview.empty());
}

void ObjectTransactiontestUnit::test_update()
{
  matador::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

  matador::transaction tr(store);

  try {
    tr.begin();


    UNIT_ASSERT_EQUAL(hans->height(), 180U);
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U);
    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr.commit();
  } catch (std::exception &) {
    UNIT_FAIL("shouldn't come here");
  }

  UNIT_ASSERT_EQUAL(hans->height(), 183U);
  UNIT_ASSERT_GREATER(hans->id(), 0UL);

  matador::object_view<person> pview(store);

  UNIT_ASSERT_FALSE(pview.empty());
}

void ObjectTransactiontestUnit::test_update_rollback()
{
  matador::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));
  hans->id(7UL);

  matador::transaction tr(store);

  try {
    tr.begin();


    UNIT_ASSERT_EQUAL(hans->height(), 180U);
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U);
    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr.rollback();
  } catch (std::exception &) {
    UNIT_FAIL("shouldn't come here");
  }

  UNIT_ASSERT_EQUAL(hans->height(), 180U);
  UNIT_ASSERT_GREATER(hans->id(), 0UL);

  matador::object_view<person> pview(store);

  UNIT_ASSERT_FALSE(pview.empty());
}

void ObjectTransactiontestUnit::test_delete()
{
  matador::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

  UNIT_ASSERT_GREATER(hans->id(), 0UL);

  matador::transaction tr(store);

  try {
    tr.begin();

    store.remove(hans);

    tr.commit();
  } catch (std::exception &) {
    tr.rollback();
  }

  matador::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_EQUAL(p.id(), 0UL);
  UNIT_ASSERT_TRUE(p.ptr() == nullptr);
}

void ObjectTransactiontestUnit::test_delete_rollback()
{
  matador::object_store store;
  store.attach<person>("person");

  auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

  UNIT_ASSERT_GREATER(hans->id(), 0UL);

  matador::transaction tr(store);

  try {
    tr.begin();

    store.remove(hans);

    tr.rollback();
  } catch (std::exception &) {
    tr.rollback();
  }

  matador::object_view<person> pview(store);

  auto p = pview.front();
  UNIT_ASSERT_EQUAL(pview.size(), 1UL);

  UNIT_ASSERT_GREATER(p.id(), 0UL);
  UNIT_ASSERT_FALSE(p.ptr() == nullptr);
  UNIT_ASSERT_GREATER(p->id(), 0UL);
  UNIT_ASSERT_EQUAL(p->name(), "Hans");
}

void ObjectTransactiontestUnit::test_nested()
{
  matador::object_store store;
  store.attach<person>("person");

  matador::transaction tr1(store);

  try {
    tr1.begin();

    auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr1.commit();

    tr1.begin();

    UNIT_ASSERT_EQUAL(hans->height(), 180U);
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U);

    matador::transaction tr2(store);

    try {
      tr2.begin();

      UNIT_ASSERT_EQUAL(hans->height(), 183U);
      hans->height(159);
      UNIT_ASSERT_EQUAL(hans->height(), 159U);

      tr2.commit();
    } catch (std::exception &) {
      tr2.rollback();
    }

    tr1.commit();
  } catch (std::exception &) {
    tr1.rollback();
  }

  matador::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL);
  UNIT_ASSERT_EQUAL(p->name(), "Hans");
}

void ObjectTransactiontestUnit::test_nested_rollback()
{
  matador::object_store store;
  store.attach<person>("person");

  matador::transaction tr1(store);

  try {
    tr1.begin();

    auto hans = store.insert(new person("Hans", matador::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL);

    tr1.commit();

    tr1.begin();

    UNIT_ASSERT_EQUAL(hans->height(), 180U);
    hans->height(183);
    UNIT_ASSERT_EQUAL(hans->height(), 183U);

    matador::transaction tr2(store);

    try {
      tr2.begin();

      UNIT_ASSERT_EQUAL(hans->height(), 183U);
      hans->height(159);
      UNIT_ASSERT_EQUAL(hans->height(), 159U);

      tr2.rollback();

      UNIT_ASSERT_EQUAL(hans->height(), 183U);

    } catch (std::exception &) {
      tr2.rollback();
    }

    tr1.commit();
  } catch (std::exception &) {
    tr1.rollback();
  }

  matador::object_view<person> pview(store);

  auto p = pview.front();

  UNIT_ASSERT_GREATER(p->id(), 0UL);
  UNIT_ASSERT_EQUAL(p->name(), "Hans");
}

void ObjectTransactiontestUnit::test_foreign()
{
  matador::object_store store;
  store.attach<child>("child");
  store.attach<master>("master");

  auto ch1 = store.insert(new child("child 1"));
  auto m1 = store.insert(new master("m1"));
  m1->children = ch1;

  matador::transaction tr(store);

  try {
    tr.begin();

    matador::object_view<master> mview(store);

    m1 = mview.front();

    UNIT_ASSERT_TRUE(m1.ptr() != nullptr);
    UNIT_ASSERT_TRUE(m1->children.ptr() != nullptr);
    UNIT_ASSERT_EQUAL(m1->name, "m1");

    UNIT_ASSERT_TRUE(store.is_removable(m1));

    store.remove(m1);

    UNIT_ASSERT_TRUE(mview.empty());

    tr.commit();
  } catch (std::exception &) {
    tr.rollback();
  }
}

void ObjectTransactiontestUnit::test_foreign_rollback()
{
  matador::object_store store;
  store.attach<child>("child");
  store.attach<master>("master");

  auto ch1 = store.insert(new child("child 1"));
  auto m1 = store.insert(new master("m1"));
  m1->children = ch1;

  matador::transaction tr(store);

  matador::object_view<master> mview(store);

  m1 = mview.front();

  try {
    tr.begin();

    UNIT_ASSERT_TRUE(m1.ptr() != nullptr);
    UNIT_ASSERT_TRUE(m1->children.ptr() != nullptr);
    UNIT_ASSERT_EQUAL(m1->name, "m1");

    UNIT_ASSERT_TRUE(store.is_removable(m1));

    store.remove(m1);

    UNIT_ASSERT_TRUE(mview.empty());

    tr.rollback();
  } catch (std::exception &) {
    tr.rollback();
  }

  UNIT_ASSERT_FALSE(mview.empty());

}
