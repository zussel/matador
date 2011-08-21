#include "object/object_store.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/object_view.hpp"
#include "object/object_serializer.hpp"

#include "unit/unit_test.hpp"
#include "unit/test_suite.hpp"

#include "ObjectStoreTestUnit.hpp"
#include "ObjectPrototypeTestUnit.hpp"
#include "ComplexObjectStoreTestUnit.hpp"

#include <iostream>

using namespace std;
using namespace oos;

int
main(int argc, char *argv[])
{
  test_suite::instance().register_unit("prototype", new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit("objects", new ObjectStoreTestUnit());
  test_suite::instance().register_unit("complex", new ComplexObjectStoreTestUnit());
	
  test_suite::instance().init(argc, argv);
	test_suite::instance().run();

	/*
	object_store ostore;
	
  try {
    // create database and make object store known to the database
//    database *db = new database(ostore, "sqlite://");
    
    // create object hierarchy
    // this leads either to a new table or if the table exists
    // data of the table will be read in
    ostore.insert_prototype(new object_producer<Item>, "ITEM");
	  ostore.insert_prototype(new object_producer<Artist>, "ARTIST");
	  ostore.insert_prototype(new object_producer<Track>, "TRACK");
    ostore.insert_prototype(new object_producer<AlbumTrack>, "ALBUMTRACK");
	  ostore.insert_prototype(new object_producer<MediaTrack>, "MEDIATRACK", "TRACK");
	  ostore.insert_prototype(new object_producer<AudioTrack>, "AUDIOTRACK", "MEDIATRACK");
	  ostore.insert_prototype(new object_producer<VideoTrack>, "VIDEOTRACK", "MEDIATRACK");
	  ostore.insert_prototype(new object_producer<Album>, "ALBUM");
    
    // show object hierarchy
	  ostore.dump_prototypes(cout);
 
 / *   
    // create new transaction    
    transaction tr(db);
    try {
      // begin transaction
      tr.begin();
      
      // ... do some object modifications
      
      
      // commit modifications
      tr.commit();
    } catch (db_exception &ex) {
      // error, abort transaction
      tr.abort();
    }
    // explicit write data to file
    db->write();
  * /
  } catch (object_exception &) {
    // output exception message
  }
  //obag.dump_objects(cout);
  */
}
