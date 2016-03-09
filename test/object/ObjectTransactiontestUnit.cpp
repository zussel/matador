//
// Created by sascha on 3/9/16.
//

#include "ObjectTransactiontestUnit.hpp"

#include "../Item.hpp"

#include "object/object_store.hpp"
#include "object/transaction.hpp"

ObjectTransactiontestUnit::ObjectTransactiontestUnit()
  : unit_test("transaction", "transaction unit test")
{
  add_test("insert", std::bind(&ObjectTransactiontestUnit::test_insert_transaction, this), "test insert transaction");
}


void ObjectTransactiontestUnit::test_insert_transaction()
{
  oos::object_store store;

  oos::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert(new person("Hans", oos::date(12, 3, 1980), 180));

    UNIT_ASSERT_GREATER(hans->id(), 0UL, "id must be valid");

    tr.commit();
  } catch (std::exception &ex) {
    tr.rollback();
  }

}
