/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DatabaseTestUnit.hpp"

#include "../Item.hpp"

#include "object/object_view.hpp"
#include "object/object_list.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/database_exception.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace oos;
using namespace std;

DatabaseTestUnit::DatabaseTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
{
  add_test("open_close", std::tr1::bind(&DatabaseTestUnit::test_open_close, this), "open database test");
  add_test("create_drop", std::tr1::bind(&DatabaseTestUnit::test_create_drop, this), "create drop database test");
  add_test("reopen", std::tr1::bind(&DatabaseTestUnit::test_reopen, this), "reopen database test");
  add_test("datatypes", std::tr1::bind(&DatabaseTestUnit::test_datatypes, this), "test all supported datatypes");
  add_test("insert", std::tr1::bind(&DatabaseTestUnit::test_insert, this), "insert an item into the database");
  add_test("update", std::tr1::bind(&DatabaseTestUnit::test_update, this), "update an item on the database");
  add_test("delete", std::tr1::bind(&DatabaseTestUnit::test_delete, this), "delete an item from the database");
//  add_test("drop", std::tr1::bind(&DatabaseTestUnit::test_drop, this), "drop database test");
  add_test("simple", std::tr1::bind(&DatabaseTestUnit::test_simple, this), "simple database test");
  add_test("complex", std::tr1::bind(&DatabaseTestUnit::test_with_sub, this), "object with sub object database test");
  add_test("list", std::tr1::bind(&DatabaseTestUnit::test_with_list, this), "object with object list database test");
  add_test("vector", std::tr1::bind(&DatabaseTestUnit::test_with_vector, this), "object with object vector database test");
  add_test("reload_simple", std::tr1::bind(&DatabaseTestUnit::test_reload_simple, this), "simple reload database test");
  add_test("reload", std::tr1::bind(&DatabaseTestUnit::test_reload, this), "reload database test");
  add_test("reload_container", std::tr1::bind(&DatabaseTestUnit::test_reload_container, this), "reload object list database test");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("item");
  ostore_.insert_prototype<ObjectItem<Item>, Item>("object_item");
  ostore_.insert_prototype<ItemPtrList>("item_ptr_list");
  ostore_.insert_prototype<ItemPtrVector>("item_ptr_vector");
//  ostore_.insert_prototype<playlist>("playlist");
  ostore_.insert_prototype<album>("album");
  ostore_.insert_prototype<track>("track");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear(true);
}

void
DatabaseTestUnit::test_open_close()
{
  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");
  
  delete db;
}

void
DatabaseTestUnit::test_create_drop()
{
  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->create();

  db->drop();

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");
  
  delete db;
}

void DatabaseTestUnit::test_reopen()
{
  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->close();

  db->open();
  
  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");
  
  delete db;
}

void DatabaseTestUnit::test_datatypes()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  try {
    // load data
    db->create();

    // load data
    db->load();
  } catch (exception &ex) {
    UNIT_FAIL("couldn't create and load database: " << ex.what());
  }

  float dval = 2.445566;
  double fval = 11111.23433345;
  char cval = 'c';
  short sval = -128;
  int ival = -49152;
  long lval = -123456789;
  unsigned short usval = 255;
  unsigned int uival = 49152;
  unsigned long ulval = 765432182;
  bool bval = true;
  oos::varchar<32> vval("hallo welt");
  std::string strval = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";

  try {

    Item *i = new Item();
    
    // set values
    i->set_bool(bval);
    i->set_char(cval);
    i->set_double(dval);
    i->set_float(fval);
    i->set_short(sval);
    i->set_int(ival);
    i->set_long(lval);
    i->set_unsigned_short(usval);
    i->set_unsigned_int(uival);
    i->set_unsigned_long(ulval);
    i->set_varchar(vval);
    i->set_string(strval);
    
    item_ptr item = db->insert(i);
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
  }
  // close db
  db->close();

  // clear object store
  ostore_.clear();

  db->open();
  
  // load data
  db->load();

  try {
    oview_t oview(ostore_);
    
    item_ptr item = oview.front();

    UNIT_ASSERT_EQUAL(item->get_char(), cval, "character is not equal");
    UNIT_ASSERT_EQUAL(item->get_short(), sval, "short is not equal");
    UNIT_ASSERT_EQUAL(item->get_int(), ival, "integer is not equal");
    UNIT_ASSERT_EQUAL(item->get_long(), lval, "long is not equal");
    UNIT_ASSERT_EQUAL(item->get_unsigned_short(), usval, "unsigned short is not equal");
    UNIT_ASSERT_EQUAL(item->get_unsigned_int(), uival, "unsigned integer is not equal");
    UNIT_ASSERT_EQUAL(item->get_unsigned_long(), ulval, "unsigned long is not equal");
    UNIT_ASSERT_EQUAL(item->get_bool(), bval, "bool is not equal");
    UNIT_ASSERT_EQUAL(item->get_string(), strval, "strings is not equal");
    UNIT_ASSERT_EQUAL(item->get_varchar(), vval, "varchar is not equal");

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
  }
  db->drop();
  // close db
  db->close();

  delete db;
}

void
DatabaseTestUnit::test_drop()
{
  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->drop();

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");
  
  delete db;
}

void DatabaseTestUnit::test_insert()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->create();

  item_ptr item = db->insert(new Item());

  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");

  ostore_.clear();
  
  db->open();
  
  db->load();
  
  oview_t oview(ostore_);

  try {
    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }

  db->drop();
  
  db->close();

  delete db;
}

void DatabaseTestUnit::test_update()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->create();

  item_ptr item = db->insert(new Item());
  
  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  item->set_string("Mars");
  
  db->update(item);

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");

  ostore_.clear();
  
  db->open();
  
  db->load();
  
  oview_t oview(ostore_);
  
  item = oview.front();
  
  UNIT_ASSERT_EQUAL("Mars", item->get_string(), "expected string must be 'Mars'");

  db->drop();

  db->close();

  delete db;
}

void DatabaseTestUnit::test_delete()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  UNIT_ASSERT_TRUE(db->is_open(), "couldn't open database database");
  
  db->create();

  item_ptr item = db->insert(new Item());

  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  db->close();

  UNIT_ASSERT_FALSE(db->is_open(), "couldn't close database database");

  ostore_.clear();
  
  db->open();
  
  db->load();
  
  oview_t oview(ostore_);

  try {
    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }

  item = oview.front();

  db->remove(item);

  try {
    UNIT_ASSERT_TRUE(oview.begin() == oview.end(), "object view must be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }

  db->drop();

  db->close();

  delete db;
}

void
DatabaseTestUnit::test_simple()
{
  // create database and make object store known to the database
  session *db = create_session();

  // create schema
  db->create();

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();

    // ... do some object modifications
    typedef object_ptr<Item> item_ptr;
    typedef object_view<Item> item_view;
    // insert new object
    item_ptr item = ostore_.insert(new Item("Hello World", 70));
    UNIT_ASSERT_GREATER(item->id(), 0, "item has invalid object id");
    tr.commit();

    tr.begin();
    // modify object
    item->set_int(120);
    UNIT_ASSERT_EQUAL(item->get_int(), 120, "item has invalid int value");
    
    transaction tr2(*db);
    try {
      // begin inner transaction
      tr2.begin();
      // change name again
      item->set_int(170);

      UNIT_ASSERT_EQUAL(item->get_int(), 170, "item has invalid int value");
      // rollback transaction
      tr2.rollback();

      UNIT_ASSERT_EQUAL(item->get_int(), 120, "item has invalid int value");
    } catch (exception &ex) {
      UNIT_WARN("transaction [" << tr2.id() << "] rolled back: " << ex.what());
      tr2.rollback();
    }
    tr.rollback();

    UNIT_ASSERT_EQUAL(item->get_int(), 70, "item has invalid int value");

    tr.begin();
    // delete object
    UNIT_ASSERT_TRUE(ostore_.is_removable(item), "couldn't delete item");
    
    ostore_.remove(item);

    tr.rollback();

    item_view view(ostore_);

    UNIT_ASSERT_FALSE(view.empty(), "item view is empty");
    UNIT_ASSERT_EQUAL((int)view.size(), 1, "more than one item in view");

    item = view.front();

    UNIT_ASSERT_EQUAL(item->get_string(), "Hello World", "invalid item name");
    UNIT_ASSERT_EQUAL(item->get_int(), 70, "invalid item int value");

    tr.begin();

    UNIT_ASSERT_TRUE(ostore_.is_removable(item), "couldn't delete item");
    
    ostore_.remove(item);
    
    tr.commit();

    UNIT_ASSERT_TRUE(view.empty(), "item view is empty");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("transaction [" << tr.id() << "] rolled back: " << ex.what());
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_with_sub()
{
  // create database and make object store known to the database
  session *db = create_session();

  // load data
  db->create();

  // load data
  /****************
   *
   * comment this statement and the following
   * will happen if data exists:
   * data won't be load
   * a car object with id 1 is created
   * and an exception on insert is thrown
   * because there is already a car object
   * with id 1
   ****************/
  db->load();

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    typedef ObjectItem<Item> object_item_t;
    typedef object_ptr<object_item_t> object_item_ptr;
    typedef object_ptr<Item> item_ptr;
    // insert new object
    object_item_ptr object_item = ostore_.insert(new object_item_t("Foo", 42));

    UNIT_ASSERT_GREATER(object_item->id(), 0, "invalid object item");

    item_ptr item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

    item->set_int(120);
    item->set_string("Bar");

    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    tr.commit();
    
    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    tr.begin();
    
    object_view<object_item_t> oview(ostore_);

    UNIT_ASSERT_FALSE(oview.empty(), "object item view couldn't be empty");

    UNIT_ASSERT_TRUE(ostore_.is_removable(object_item), "couldn't remove object item");

    ostore_.remove(object_item);
    
    UNIT_ASSERT_TRUE(oview.empty(), "object item view must be empty");
    
    tr.rollback();

    UNIT_ASSERT_FALSE(oview.empty(), "object item view couldn't be empty");

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_with_list()
{
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef ItemPtrList::value_type item_ptr;

  // create database and make object store known to the database
  session *db = create_session();

  // load data
  db->create();

  // load db
  db->load();

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications

    itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);

    UNIT_ASSERT_GREATER(itemlist->id(), 0, "invalid item list");
    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemlist->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

    tr.rollback();

    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "invalid item list size");

    tr.begin();

    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemlist->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

    tr.commit();
    
    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");
    tr.begin();
    
    itemlist->clear();

    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");

    tr.rollback();

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_with_vector()
{
  typedef object_ptr<ItemPtrVector> itemvector_ptr;
  typedef ItemPtrVector::value_type item_ptr;

  // create database and make object store known to the database
  session *db = create_session();

  // load data
  db->create();

  // load db
  db->load();

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications

    itemvector_ptr itemvector = ostore_.insert(new ItemPtrVector);

    UNIT_ASSERT_GREATER(itemvector->id(), 0, "invalid item list");
    UNIT_ASSERT_TRUE(itemvector->empty(), "item list must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemvector->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemvector->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemvector->size(), 2, "invalid item list size");

    tr.rollback();

    UNIT_ASSERT_TRUE(itemvector->empty(), "item list must be empty");
    UNIT_ASSERT_EQUAL((int)itemvector->size(), 0, "invalid item list size");

    tr.begin();

    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemvector->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemvector->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemvector->size(), 2, "invalid item list size");

    tr.commit();
    
    UNIT_ASSERT_FALSE(itemvector->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemvector->size(), 2, "invalid item list size");
    tr.begin();
    
    itemvector->clear();

    UNIT_ASSERT_TRUE(itemvector->empty(), "item list must be empty");

    tr.rollback();

    UNIT_ASSERT_FALSE(itemvector->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemvector->size(), 2, "invalid item list size");

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_reload_simple()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  try {
    // load data
    db->create();

    // load data
    db->load();
  } catch (exception &ex) {
    UNIT_FAIL("couldn't create and load database: " << ex.what());
  }

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    // insert new object
    item_ptr item = ostore_.insert(new Item("Foo", 42));

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid object item");

    item = ostore_.insert(new Item("Bar", 99));

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid object item");

    tr.commit();
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db->close();

  // clear object store
  ostore_.clear();

  db->open();
  
  // load data
  db->load();

  try {
    // create view
    oview_t oview(ostore_);
    // check iterators
    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
    // check emptiness
    UNIT_ASSERT_FALSE(oview.empty(), "object view must not be empty");
    // check size
    UNIT_ASSERT_TRUE(oview.size() == 2, "object view size must be 2");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_reload()
{
  typedef ObjectItem<Item> object_item_t;
  typedef object_ptr<object_item_t> object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_view<object_item_t> oview_t;

  // create database and make object store known to the database
  session *db = create_session();

  try {
    // load data
    db->create();

    // load data
    db->load();
  } catch (exception &ex) {
    UNIT_FAIL("couldn't create and load database: " << ex.what());
  }

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    // insert new object
    object_item_ptr object_item = ostore_.insert(new object_item_t("Foo", 42));

    UNIT_ASSERT_GREATER(object_item->id(), 0, "invalid object item");

    item_ptr item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

    item->set_int(120);
    item->set_string("Bar");

    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    item->set_cstr("hello", 5);

    oview_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");

    tr.commit();
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db->close();

  // clear object store
  ostore_.clear();

  db->open();
  
  // load data
  db->load();

  try {
    oview_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

void
DatabaseTestUnit::test_reload_container()
{
  typedef object_ptr<album> album_ptr;
  typedef object_ptr<track> track_ptr;
  
  // create database and make object store known to the database
  session *db = create_session();

  // load data
  db->create();

  // load db
  db->load();

  // create new transaction    
  transaction tr(*db);
  try {
    // begin transaction
    tr.begin();

    album_ptr alb1 = ostore_.insert(new album("My Album"));

    UNIT_ASSERT_GREATER(alb1->id(), 0, "invalid album");
    UNIT_ASSERT_TRUE(alb1->empty(), "album must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 5; ++i) {
      stringstream name;
      name << "Track " << i+1;

      track_ptr trk = ostore_.insert(new track(name.str()));
      UNIT_ASSERT_GREATER(trk->id(), 0, "invalid track");

      alb1->add(trk);
//      cout << "\nadded track: " << trk->title() << " (index: " << trk->index() << ", album: " << trk->alb()->id() << ")";
    }

    alb1->insert(alb1->begin() + 2, ostore_.insert(new track("Track 6")));

    tr.commit();
/*
    cout << "\n";

    album::const_iterator first = alb1->begin();
    album::const_iterator last = alb1->end();
    while (first != last) {
      track_ptr tr1 = *first++;
      cout << "track: " << tr1->title() << " (index: " << tr1->index() << ")\n";
    }

    cout << "\nsize: " << alb1->size() << "\n";
*/
    UNIT_ASSERT_FALSE(alb1->empty(), "album couldn't be empty");
    UNIT_ASSERT_EQUAL((int)alb1->size(), 6, "invalid album size");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

//  std::ofstream out("prototype.dot", ios_base::out | ios_base::trunc);
  
//  ostore_.dump_prototypes(out);

  // close db
  db->close();
  
  // clear object store
  ostore_.clear();

  db->open();

//  cout << "reloading \n";
  // load data
  db->load();

  try {
    typedef object_view<album> album_view_t;
    album_view_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "album view must not be empty");
    
    album_ptr alb1 = *oview.begin();
    
//    cout << "size: " << alb1->size() << "\n";
    
    UNIT_ASSERT_FALSE(alb1->empty(), "album couldn't be empty");
    UNIT_ASSERT_EQUAL((int)alb1->size(), 6, "invalid album size");
/*
    album::const_iterator first = alb1->begin();
    album::const_iterator last = alb1->end();
    while (first != last) {
      track_ptr tr1 = *first++;
      cout << "track: " << tr1->title() << " (index: " << tr1->index() << ")\n";
    }
*/
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  db->drop();
  // close db
  db->close();
  
  delete db;
}

session* DatabaseTestUnit::create_session()
{
  return new session(ostore_, db_);
}

oos::object_store& DatabaseTestUnit::ostore()
{
  return ostore_;
}

const oos::object_store& DatabaseTestUnit::ostore() const
{
  return ostore_;
}
