#include "TransactionTestUnit.hpp"

#include "../datatypes.hpp"
#include "../person.hpp"
#include "../entities.hpp"

#include "matador/object/object_view.hpp"

#include "matador/orm/session.hpp"

#include "matador/sql/sql_exception.hpp"

using namespace matador;


TransactionTestUnit::TransactionTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_transaction", prefix + " transaction test unit")
  , dns_(std::move(dns))
{
  add_test("simple", std::bind(&TransactionTestUnit::test_simple, this), "simple transaction test");
  add_test("nested", std::bind(&TransactionTestUnit::test_nested, this), "nested transaction test");
  add_test("foreign", std::bind(&TransactionTestUnit::test_foreign, this), "object with foreign key transaction test");
  add_test("list_commit", std::bind(&TransactionTestUnit::test_has_many_list_commit, this), "object with object list transaction commit test");
  add_test("list_rollback", std::bind(&TransactionTestUnit::test_has_many_list_rollback, this), "object with object list transaction rollback test");
  add_test("list", std::bind(&TransactionTestUnit::test_has_many_list, this), "object with object list transaction test");
  add_test("vector", std::bind(&TransactionTestUnit::test_has_many_vector, this), "object with object vector transaction test");
//  add_test("vector", std::bind(&TransactionTestUnit::test_with_vector, this), "serializable with serializable vector sql test");
}


void TransactionTestUnit::test_simple()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  transaction tr = s.begin();

  matador::date d1(21, 12, 1980);
  try {
    auto hans = s.insert(new person("hans", d1, 180));

    tr.commit();
  } catch (sql_exception &ex) {
    tr.rollback();
    UNIT_FAIL("transaction failed: " << ex.what());
  }

//  matador::object_view<person> persons(s.store());
  matador::object_view<person> persons = s.select<person>();

  UNIT_ASSERT_EQUAL(1UL, persons.size());

  auto hans2 = persons.front();

  UNIT_ASSERT_EQUAL("hans", hans2->name());
  UNIT_ASSERT_EQUAL(d1, hans2->birthdate());
  UNIT_ASSERT_EQUAL(180U, hans2->height());

  p.drop();
}

void TransactionTestUnit::test_nested()
{
  matador::persistence p(dns_);

  p.attach<datatypes>("item");

  p.create();

  matador::session s(p);

  // create and begin transaction
  transaction tr = s.begin();

  try {
    // ... do some serializable modifications
    typedef object_ptr<datatypes> item_ptr;
    typedef object_view<datatypes> item_view;
    // insert new serializable
    item_ptr item = s.store().insert(new datatypes("Hello World", 70));
    UNIT_ASSERT_GREATER(item->id(), 0UL);
    tr.commit();

    tr.begin();
    // modify serializable
    item.modify()->set_int(120);
    UNIT_ASSERT_EQUAL(item->get_int(), 120);


    // begin inner transaction
    transaction tr2 = s.begin();
    try {
      // change name again
      item.modify()->set_int(170);

      UNIT_ASSERT_EQUAL(item->get_int(), 170);
      // rollback transaction
      tr2.rollback();

      UNIT_ASSERT_EQUAL(item->get_int(), 120);
    } catch (std::exception &ex) {
      UNIT_WARN("transaction [" << tr2.id() << "] rolled back: " << ex.what());
      tr2.rollback();
    }
    tr.rollback();

    UNIT_ASSERT_EQUAL(item->get_int(), 70);

    tr.begin();
    // delete serializable
    UNIT_ASSERT_TRUE(s.store().is_removable(item));

    s.store().remove(item);

    tr.rollback();

    item_view view(s.store());

    UNIT_ASSERT_FALSE(view.empty());
    UNIT_ASSERT_EQUAL((int)view.size(), 1);

    item = view.front();

    UNIT_ASSERT_EQUAL(item->get_string(), "Hello World");
    UNIT_ASSERT_EQUAL(item->get_int(), 70);

    tr.begin();

    UNIT_ASSERT_TRUE(s.store().is_removable(item));

    s.store().remove(item);

    tr.commit();

    UNIT_ASSERT_TRUE(view.empty());
  } catch (sql_exception &ex) {
    // error, abort transaction
    UNIT_WARN("transaction [" << tr.id() << "] rolled back: " << ex.what());
    tr.rollback();
  }

  p.drop();
}

void TransactionTestUnit::test_foreign()
{
  matador::persistence p(dns_);

  p.attach<datatypes>("item");
  p.attach<ObjectItem<datatypes>>("object_item");

  p.create();

  matador::session s(p);

  // load data
  /****************
  *
  * comment this statement and the following
  * will happen if data exists:
  * data won't be load
  * a car serializable with id 1 is created
  * and an exception on insert is thrown
  * because there is already a car serializable
  * with id 1
  ****************/
//  session_->load();

  // create and begin transaction
  transaction tr = s.begin();
  try {
    // ... do some serializable modifications
    typedef ObjectItem<datatypes> object_item_t;
    typedef object_ptr<object_item_t> object_item_ptr;
    typedef object_ptr<datatypes> item_ptr;
    // insert new serializable
    item_ptr item = s.insert(new datatypes("Bar", 13));
    object_item_ptr object_item = s.insert(new object_item_t("Foo", 42));
    object_item.modify()->ptr(item);

    UNIT_ASSERT_GREATER(object_item->id(), 0UL);

    item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0UL);

    item.modify()->set_int(120);
    item.modify()->set_string("Bar");

    UNIT_ASSERT_EQUAL(item->get_int(), 120);
    //UNIT_ASSERT_EQUAL(item->get_string(), "Bar");

    tr.commit();

    UNIT_ASSERT_EQUAL(item->get_int(), 120);
    //UNIT_ASSERT_EQUAL(item->get_string(), "Bar");

    tr.begin();

    object_view<object_item_t> oview(s.store());

    UNIT_ASSERT_FALSE(oview.empty());

    UNIT_ASSERT_TRUE(s.store().is_removable(object_item));

    s.store().remove(object_item);

    UNIT_ASSERT_TRUE(oview.empty());

    tr.rollback();

    UNIT_ASSERT_FALSE(oview.empty());

  } catch (std::exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

  p.drop();
}

void TransactionTestUnit::test_has_many_list_commit()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  transaction tr = s.begin();
  try {
    auto children = s.insert(new children_list("children list"));

    UNIT_ASSERT_GREATER(children->id, 0UL);
    UNIT_ASSERT_TRUE(children->children.empty());

    tr.commit();

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.commit();

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

  } catch(std::exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught exception: " << ex.what() << " (start rollback)");
  }

  p.drop();
}

void TransactionTestUnit::test_has_many_list_rollback()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  transaction tr = s.begin();
  try {
    auto children = s.insert(new children_list("children list"));

    UNIT_ASSERT_GREATER(children->id, 0UL);
    UNIT_ASSERT_TRUE(children->children.empty());

    tr.commit();

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.rollback();

    UNIT_ASSERT_TRUE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 0UL);

  } catch(std::exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

  p.drop();
}

void TransactionTestUnit::test_has_many_list()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  transaction tr = s.begin();
  try {
    auto children = s.insert(new children_list("children list"));

    UNIT_ASSERT_GREATER(children->id, 0UL);
    UNIT_ASSERT_TRUE(children->children.empty());

    tr.commit();

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.rollback();

    UNIT_ASSERT_TRUE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 0UL);

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.commit();

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.begin();

    children.modify()->children.clear();

    UNIT_ASSERT_TRUE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 0UL);

    tr.rollback();

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

  } catch(std::exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

  p.drop();
}

void TransactionTestUnit::test_has_many_vector()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_vector>("children_vector");

  p.create();

  matador::session s(p);

  transaction tr = s.begin();
  try {
    auto children = s.insert(new children_vector("children vector"));

    UNIT_ASSERT_GREATER(children->id, 0UL);
    UNIT_ASSERT_TRUE(children->children.empty());

    tr.commit();

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.rollback();

    UNIT_ASSERT_TRUE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 0UL);

    tr.begin();

    for (int i = 1; i <= 2; ++i) {
      std::stringstream name;
      name << "child " << i;
      auto kid = s.insert(new child(name.str()));

      UNIT_ASSERT_GREATER(kid->id, 0UL);

      children.modify()->children.push_back(kid);
    }

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.commit();

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

    tr.begin();

    children.modify()->children.clear();

    UNIT_ASSERT_TRUE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 0UL);

    tr.rollback();

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

  } catch(std::exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

  p.drop();
}

