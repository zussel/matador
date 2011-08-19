#include "object/object_store.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/object_view.hpp"
#include "object/object_serializer.hpp"

#include "unit/unit_test.hpp"
#include "unit/test_suite.hpp"

#include "Track.hpp"
#include "Album.hpp"
#include "Artist.hpp"
#include "ComplexObjectStoreTestUnit.hpp"

#include <iostream>

using namespace std;
using namespace oos;

class ObjectPrototypeTestUnit : public unit_test
{
public:
  ObjectPrototypeTestUnit()
    : unit_test("ObjectStore Prototype Test Unit")
  {
		add_test("one", std::tr1::bind(&ObjectPrototypeTestUnit::one_prototype, this), "one prototype");
		add_test("hierarchy", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_hierachy, this), "prototype hierarchy");
  }
  virtual ~ObjectPrototypeTestUnit() {}
  
  virtual void initialize() {}
  virtual void finalize() {}
  
  void one_prototype()
  {
		object_store ostore;
		ostore.insert_prototype(new object_producer<Artist>, "ARTIST");
    
    object *o = ostore.create("ARTIST");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <Artist>");
    
    Artist *a = dynamic_cast<Artist*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to Artist");
    
    delete a;
    
    ostore.remove_prototype("ARTIST");
    
    o = ostore.create("ARTIST");
    
    UNIT_ASSERT_NULL(o, "unexpected object creation");
	}
  void prototype_hierachy()
  {
		object_store ostore;
		ostore.insert_prototype(new object_producer<Track>, "TRACK");
		ostore.insert_prototype(new object_producer<MediaTrack>, "MEDIATRACK", "TRACK");
		ostore.insert_prototype(new object_producer<AudioTrack>, "AUDIOTRACK", "TRACK");
		ostore.insert_prototype(new object_producer<VideoTrack>, "VIDEOTRACK", "TRACK");

    object *o = ostore.create("AUDIOTRACK");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <AudioTrack>");
    
    AudioTrack *a = dynamic_cast<AudioTrack*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to AudioTrack");
    
    delete a;
    
    ostore.remove_prototype("AUDIOTRACK");
    
    o = ostore.create("AUDIOTRACK");
    
    UNIT_ASSERT_NULL(o, "unexpected object creation");
    
    ostore.remove_prototype("TRACK");
    
    o = ostore.create("MEDIATRACK");
    
    UNIT_ASSERT_NULL(o, "unexpected object creation");
  }
};

class ObjectStoreTestUnit : public unit_test
{
private:
  class SimpleObject : public object
  {
  public:
    SimpleObject() {}
    SimpleObject(const std::string &n) : name_(n) {}
    virtual ~SimpleObject() {}
    
    void read_from(object_atomizer *reader)
    {
      object::read_from(reader);
      reader->read_string("name", name_);
    }
    void write_to(object_atomizer *writer)
    {
      object::write_to(writer);
      writer->write_string("name", name_);
    }

    std::string name() const { return name_; }
    void name(const std::string &n) { name_ = n; }

  private:
    std::string name_;
  };
  class ObjectWithSubObject : public object
  {
  public:
    ObjectWithSubObject() {}
    virtual ~ObjectWithSubObject() {}

    void read_from(object_atomizer *reader)
    {
      object::read_from(reader);
      reader->read_object("simple", simple_);
    }
    void write_to(object_atomizer *writer)
    {
      object::write_to(writer);
      writer->write_object("simple", simple_);
    }

    object_ptr<SimpleObject> simple() const { return simple_; }
    void simple(const object_ptr<SimpleObject> &s) { simple_ = s; }

  private:
    object_ptr<SimpleObject> simple_;
  };

public:
  ObjectStoreTestUnit()
    : unit_test("ObjectStore Test Unit")
  {
		add_test("simple", std::tr1::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
		add_test("with_sub", std::tr1::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete object with sub object");
		add_test("multiple_simple", std::tr1::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
		add_test("multiple_object_with_sub", std::tr1::bind(&ObjectStoreTestUnit::multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
  }
  virtual ~ObjectStoreTestUnit() {}
  
  virtual void initialize()
  {
		ostore_.insert_prototype(new object_producer<SimpleObject>, "SIMPLE_OBJECT");
		ostore_.insert_prototype(new object_producer<ObjectWithSubObject>, "OBJECT_WITH_SUB_OBJECT");
  }
  virtual void finalize()
  {
    ostore_.clear();
  }
  
  void simple_object()
  {
    object *o = ostore_.create("SIMPLE_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
    
    SimpleObject *a = dynamic_cast<SimpleObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
    
    typedef object_ptr<SimpleObject> simple_ptr;
    
    simple_ptr simple = ostore_.insert(a);
    
    UNIT_ASSERT_NOT_NULL(simple.get(), "simple object insertion failed");
    
    UNIT_ASSERT_TRUE(ostore_.remove(simple), "deletion of simple failed");
  }
  void object_with_sub_object()
  {
    object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
    
    ObjectWithSubObject *s = dynamic_cast<ObjectWithSubObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(s, "couldn't cast object to ObjectWithSubObject");
    
    typedef object_ptr<ObjectWithSubObject> obj_with_sub_ptr;
    
    obj_with_sub_ptr ows = ostore_.insert(s);
    
    UNIT_ASSERT_NOT_NULL(ows.get(), "object with sub object insertion failed");
    
    // check if sub object exists
    object_ptr<SimpleObject> simple = ows->simple();
    
    UNIT_ASSERT_NOT_NULL(simple.get(), "simple sub object creation failed");
    
    UNIT_ASSERT_TRUE(ostore_.remove(ows), "deletion of object with sub object failed");
  }
  void multiple_simple_objects()
  {
    typedef object_ptr<SimpleObject> simple_ptr;
      
    // create 1000 objects
    UNIT_INFO("\ncreating objects :");
    for (int i = 0; i < 100000; ++i) {
      if (i%1000 == 0) {
        std::cout << " " << i;
        std::cout.flush();
      }
      object *o = ostore_.create("SIMPLE_OBJECT");
      
      UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
      
      SimpleObject *a = dynamic_cast<SimpleObject*>(o);
      
      UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
      
      simple_ptr simple = ostore_.insert(a);
      
      //ostore_.remove(simple);
    }
    UNIT_INFO(" done\n");
    
    /*
    std::cout << std::endl;
    SimpleObject *pso;
    std::cout << "typeid (pso*)   : " << typeid(pso).name() << std::endl;
    std::cout << "typeid (SimpleObject*)   : " << typeid(SimpleObject*).name() << std::endl;
    std::cout << "typeid (SimpleObject)    : " << typeid(SimpleObject).name() << std::endl;
    SimpleObject so;
    std::cout << "typeid (SimpleObject  so): " << typeid(so).name() << std::endl;
    std::cout << "typeid (SimpleObject *so): " << typeid(&so).name() << std::endl;
    */
  }
  void multiple_object_with_sub_objects()
  {
    typedef object_ptr<ObjectWithSubObject> ows_ptr;
      
    // create 1000 objects
    UNIT_INFO("\ncreating objects :");
    for (int i = 0; i < 100000; ++i) {
      if (i%1000 == 0) {
        std::cout << " " << i;
        std::cout.flush();
      }

      object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
      
      UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
      
      ObjectWithSubObject *a = dynamic_cast<ObjectWithSubObject*>(o);
      
      UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ObjectWithSubObject");
      
      ows_ptr ows = ostore_.insert(a);
      
      //ostore_.remove(simple);
    }
    UNIT_INFO(" done\n");
  }

private:
  object_store ostore_;
};

bool test_1(object_store &ostore);
bool test_2(object_store &ostore);
bool test_3(object_store &ostore);
bool test_4(object_store &ostore);

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
//  test_1(ostore);
//  test_2(ostore);
//  test_3(ostore);
  test_4(ostore);
  */
}
