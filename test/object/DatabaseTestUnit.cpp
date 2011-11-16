#include "DatabaseTestUnit.hpp"

#include "Car.hpp"
#include "Engine.hpp"

#include "object/object_view.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"

#include <iostream>
#include <fstream>

using namespace oos;
using namespace std;

DatabaseTestUnit::DatabaseTestUnit()
  : unit_test("database test unit")
{
  add_test("simple", std::tr1::bind(&DatabaseTestUnit::simple, this), "simple database test");
  add_test("with_sub", std::tr1::bind(&DatabaseTestUnit::with_sub, this), "object with sub object database test");
  add_test("test", std::tr1::bind(&DatabaseTestUnit::test, this), "simple test method");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<Car>, "CAR");
  ostore_.insert_prototype(new object_producer<Engine>, "ENGINE");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear();
}

void
DatabaseTestUnit::simple()
{
  cout << endl;
  // create database and make object store known to the database
  database *db = new database(ostore_, "sqlite://");

  // open db
  db->open();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();

    // ... do some object modifications
    typedef object_ptr<Engine> engine_ptr;
    typedef object_view<Engine> engine_view;
    // insert new object
    engine_ptr engine = ostore_.insert(new Engine(70, 4, 1.4f));
    cout << "inserted " << *engine << "\n";
    tr.commit();

    tr.begin();
    // modify object
    engine->power(120);
    cout << "changed power of " << *engine << "\n";
    
    transaction tr2(db);
    try {
      // begin inner transaction
      tr2.begin();
      // change name again
      engine->power(170);
      cout << "changed power of " << *engine << "\n";
      // rollback transaction
      tr2.rollback();
      cout << "after rollback " << *engine << "\n";
    } catch (exception &) {
      tr2.rollback();
    }
    tr.rollback();
    cout << "after rollback " << *engine << "\n";

    tr.begin();
    // delete object
    cout << "deleting " << *engine << "\n";
    ostore_.remove(engine);

    // show objects
    ostore_.dump_objects(cout);

    cout << "starting rollback \n";
    tr.rollback();
    cout << "finished rollback \n";
    cout << "after rollback \n";
    engine_view view(ostore_);
    if (view.size() > 0) {
      engine_view::iterator i = view.begin();
      cout << "got " << *i.optr() << endl;
    } else {
      cout << "ERROR: no object in engine view!\n";
    }

    // commit modifications
//    tr.commit();
/*
    // begin transaction
    tr.begin();
    // insert new object
    artist = ostore_.insert(new Artist("AC/DC"));
    // modify object
    artist->name("Beatles");
    // delete object
    ostore_.remove(artist);

//    cout << artist->name() << endl;
    // show objects
    ostore_.dump_objects(cout);

    // commit modifications
    tr.commit();
    */
  } catch (exception &) {
    // error, abort transaction
    tr.rollback();
  }
  // close db
  db->close();
  // explicit write data to file
  //db->write();
  delete db;
}

void
DatabaseTestUnit::with_sub()
{
  cout << endl;
  // create database and make object store known to the database
  database *db = new database(ostore_, "oodb://");

  // open db
  db->open();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    typedef object_ptr<Car> car_ptr;
    typedef object_ptr<Engine> engine_ptr;
    // insert new object
    car_ptr car = ostore_.insert(new Car("VW", "Beetle"));    
    engine_ptr engine = car->engine();
    ostore_.dump_objects(cout);
    engine->power(120);
    engine->cylinder(4);
    engine->capacity(1.4f);
    cout << "inserted " << *car << " with " << *engine << endl;
    tr.commit();
    
    tr.begin();
    
    cout << "removing " << *car << " ... ";
    ostore_.remove(car);
    cout << "done.\n";
    ostore_.dump_objects(cout);
    cout << "starting rollback\n";
    tr.rollback();
    cout << "finished rollback\n";
    
    ostore_.dump_objects(cout);
    
  } catch (exception &) {
    // error, abort transaction
    tr.rollback();
  }
  // close db
  db->close();
  // explicit write data to file
  //db->write();
  delete db;
}

void
DatabaseTestUnit::test()
{
  std::cout << std::endl;
  std::string str("hallo welt und alle anderen 12345678901234567890");
  std::cout << "sizeof str: " << sizeof(str) << "\n";

  std::tr1::shared_ptr<string> strp(new string("hallo welt und alle anderen 12345678901234567890"));
  std::cout << "sizeof strp: " << sizeof(strp) << "\n";
}
