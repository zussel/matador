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
  , session_(nullptr) 
{
  add_test("datatypes", std::bind(&DatabaseTestUnit::test_datatypes, this), "test all supported datatypes");
  add_test("insert", std::bind(&DatabaseTestUnit::test_insert, this), "insert an item into the database");
  add_test("update", std::bind(&DatabaseTestUnit::test_update, this), "update an item on the database");
  add_test("delete", std::bind(&DatabaseTestUnit::test_delete, this), "delete an item from the database");
  add_test("reload_simple", std::bind(&DatabaseTestUnit::test_reload_simple, this), "simple reload database test");
  add_test("reload", std::bind(&DatabaseTestUnit::test_reload, this), "reload database test");
  add_test("reload_container", std::bind(&DatabaseTestUnit::test_reload_container, this), "reload object list database test");
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
  
  // create session
  session_ = create_session();

  session_->open();

  session_->create();
}

void
DatabaseTestUnit::finalize()
{
  session_->drop();

  session_->close();

  delete session_;

  ostore_.clear(true);
}

void DatabaseTestUnit::test_datatypes()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  float fval = 2.445566f;
  double dval = 11111.23433345;
  char cval = 'c';
  short sval = -128;
  int ival = -49152;
  long lval = -123456789;
  unsigned short usval = 255;
  unsigned int uival = 49152;
  unsigned long ulval = 765432182;
  bool bval = true;
  const char *cstr("Armer schwarzer Kater");
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
    i->set_cstr(cstr, strlen(cstr) + 1);
    i->set_varchar(vval);
    i->set_string(strval);
    
    item_ptr item = session_->insert(i);
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_FAIL("caught database exception: " << ex.what() << " (start rollback)");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_FAIL("caught object exception: " << ex.what() << " (start rollback)");
  } catch (std::exception &ex) {
    // error, abort transaction
    UNIT_FAIL("caught exception: " << ex.what() << " (start rollback)");
  }
  // close db
  session_->close();

  // clear object store
  ostore_.clear();

  session_->open();
  
  // load data
  session_->load();

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
    UNIT_ASSERT_EQUAL(item->get_cstr(), cstr, "const char pointer is not equal");
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
}

void DatabaseTestUnit::test_insert()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  item_ptr item = session_->insert(new Item());

  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");

  ostore_.clear();
  
  session_->open();
  
  session_->load();
  
  oview_t oview(ostore_);

  try {
    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }
}

void DatabaseTestUnit::test_update()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  item_ptr item = session_->insert(new Item());
  
  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  item->set_string("Mars");
  
  session_->update(item);

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");

  ostore_.clear();
  
  session_->open();
  
  session_->load();
  
  oview_t oview(ostore_);
  
  item = oview.front();
  
  UNIT_ASSERT_EQUAL("Mars", item->get_string(), "expected string must be 'Mars'");
}

void DatabaseTestUnit::test_delete()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  item_ptr item = session_->insert(new Item());

  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");

  ostore_.clear();
  
  session_->open();
  
  session_->load();
  
  oview_t oview(ostore_);

  try {
    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }

  item = oview.front();

  session_->remove(item);

  try {
    UNIT_ASSERT_TRUE(oview.begin() == oview.end(), "object view must be empty");
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what());
  }
}

void
DatabaseTestUnit::test_reload_simple()
{
  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> oview_t;

  // create new transaction
  transaction tr(*session_);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    // insert new object
    item_ptr item = ostore_.insert(new Item("Foo", 42));

    UNIT_ASSERT_GREATER(item->id(), 0UL, "invalid object item");

    item = ostore_.insert(new Item("Bar", 99));

    UNIT_ASSERT_GREATER(item->id(), 0UL, "invalid object item");

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
  session_->close();

  // clear object store
  ostore_.clear();

  session_->open();
  
  // load data
  session_->load();

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
}

void
DatabaseTestUnit::test_reload()
{
  typedef ObjectItem<Item> object_item_t;
  typedef object_ptr<object_item_t> object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_view<object_item_t> oview_t;

  // create new transaction    
  transaction tr(*session_);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    // insert new object
    object_item_ptr object_item = ostore_.insert(new object_item_t("Foo", 42));

    UNIT_ASSERT_GREATER(object_item->id(), 0UL, "invalid object item");

    item_ptr item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0UL, "invalid item");

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
  session_->close();

  // clear object store
  ostore_.clear();

  session_->open();
  
  // load data
  session_->load();

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
}

void
DatabaseTestUnit::test_reload_container()
{
  typedef object_ptr<album> album_ptr;
  typedef object_ptr<track> track_ptr;
  
  // create new transaction
  transaction tr(*session_);
  try {
    // begin transaction
    tr.begin();

    album_ptr alb1 = ostore_.insert(new album("My Album"));

    UNIT_ASSERT_GREATER(alb1->id(), 0UL, "invalid album");
    UNIT_ASSERT_TRUE(alb1->empty(), "album must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 5; ++i) {
      stringstream name;
      name << "Track " << i+1;

      track_ptr trk = ostore_.insert(new track(name.str()));
      UNIT_ASSERT_GREATER(trk->id(), 0UL, "invalid track");

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
  session_->close();
  
  // clear object store
  ostore_.clear();

  session_->open();

//  cout << "reloading \n";
  // load data
  session_->load();

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
