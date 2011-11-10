#include "DatabaseTestUnit.hpp"

#include "Track.hpp"
#include "Album.hpp"
#include "Artist.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"

#include <iostream>
#include <fstream>

using namespace oos;
using namespace std;

typedef object_ref_list_node<Track> AlbumTrack;

DatabaseTestUnit::DatabaseTestUnit()
  : unit_test("database test unit")
{
  add_test("basic", std::tr1::bind(&DatabaseTestUnit::basic, this), "basic database test");
  add_test("test", std::tr1::bind(&DatabaseTestUnit::test, this), "simple test method");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<Artist>, "ARTIST");
  ostore_.insert_prototype(new object_producer<Track>, "TRACK");
  ostore_.insert_prototype(new object_producer<Album>, "ALBUM");
  ostore_.insert_prototype(new object_producer<AlbumTrack>, "ALBUMTRACK");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear();
}

void
DatabaseTestUnit::basic()
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
    typedef object_ptr<Artist> artist_ptr;
    // insert new object
    artist_ptr artist = ostore_.insert(new Artist("AC/DC"));
    cout << "inserted " << *artist << "\n";
    tr.commit();

    tr.begin();
    // modify object
    artist->name("Beatles");
    cout << "changed name of " << *artist << "\n";
    tr.rollback();
    cout << "after rollback name of " << *artist << "\n";

    tr.begin();
    // delete object
    cout << "deleting " << *artist << "\n";
    ostore_.remove(artist);

    // show objects
    ostore_.dump_objects(cout);

    tr.rollback();
    cout << "after rollback \n";
    ostore_.dump_objects(cout);

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
DatabaseTestUnit::test()
{
  std::cout << std::endl;
  std::string str("hallo welt und alle anderen 12345678901234567890");
  std::cout << "sizeof str: " << sizeof(str) << "\n";

  std::tr1::shared_ptr<string> strp(new string("hallo welt und alle anderen 12345678901234567890"));
  std::cout << "sizeof strp: " << sizeof(strp) << "\n";
}
