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

#include <iostream>

using namespace std;
using namespace oos;

class Item : public object_list_node<Item>
{
public:
  Item() {}
  Item(const std::string &name) : name_(name) {}
  virtual ~Item() {}

	void read_from(object_atomizer *reader)
  {
    object_list_node::read_from(reader);
    reader->read_string("name", name_);
  }
	void write_to(object_atomizer *writer)
  {
    object_list_node::write_to(writer);
    writer->write_string("name", name_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n) { name_ = n; }

private:
  std::string name_;
};

typedef object_ref_list_node<Track> AlbumTrack;

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
    
    ASSERT_NOT_NULL(o, "couldn't create object of type <Artist>");
    
    Artist *a = dynamic_cast<Artist*>(o);
    
    ASSERT_NOT_NULL(a, "couldn't cast object to Artist");
    
    delete a;
    
    ostore.remove_prototype("ARTIST");
    
    o = ostore.create("ARTIST");
    
    ASSERT_NULL(o, "unexpected object creation");
	}
  void prototype_hierachy()
  {
		object_store ostore;
		ostore.insert_prototype(new object_producer<Track>, "TRACK");
		ostore.insert_prototype(new object_producer<MediaTrack>, "MEDIATRACK", "TRACK");
		ostore.insert_prototype(new object_producer<AudioTrack>, "AUDIOTRACK", "TRACK");
		ostore.insert_prototype(new object_producer<VideoTrack>, "VIDEOTRACK", "TRACK");

    object *o = ostore.create("AUDIOTRACK");
    
    ASSERT_NOT_NULL(o, "couldn't create object of type <AudioTrack>");
    
    AudioTrack *a = dynamic_cast<AudioTrack*>(o);
    
    ASSERT_NOT_NULL(a, "couldn't cast object to AudioTrack");
    
    delete a;
    
    ostore.remove_prototype("AUDIOTRACK");
    
    o = ostore.create("AUDIOTRACK");
    
    ASSERT_NULL(o, "unexpected object creation");
    
    ostore.remove_prototype("TRACK");
    
    o = ostore.create("MEDIATRACK");
    
    ASSERT_NULL(o, "unexpected object creation");
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
		//add_test("simple", std::tr1::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
		//add_test("with_sub", std::tr1::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete object with sub object");
		add_test("multiple_simple", std::tr1::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
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
    
    ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
    
    SimpleObject *a = dynamic_cast<SimpleObject*>(o);
    
    ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
    
    typedef object_ptr<SimpleObject> simple_ptr;
    
    simple_ptr simple = ostore_.insert(a);
    
    ASSERT_NOT_NULL(simple.get(), "simple object insertion failed");
    
    ASSERT_TRUE(ostore_.remove(simple), "deletion of simple failed");
  }
  void object_with_sub_object()
  {
    object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
    
    ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
    
    ObjectWithSubObject *s = dynamic_cast<ObjectWithSubObject*>(o);
    
    ASSERT_NOT_NULL(s, "couldn't cast object to ObjectWithSubObject");
    
    typedef object_ptr<ObjectWithSubObject> obj_with_sub_ptr;
    
    obj_with_sub_ptr ows = ostore_.insert(s);
    
    ASSERT_NOT_NULL(ows.get(), "object with sub object insertion failed");
    
    // check if sub object exists
    object_ptr<SimpleObject> simple = ows->simple();
    
    ASSERT_NOT_NULL(simple.get(), "simple sub object creation failed");
    
    ASSERT_TRUE(ostore_.remove(ows), "deletion of object with sub object failed");
  }
  void multiple_simple_objects()
  {
    typedef object_ptr<SimpleObject> simple_ptr;
      
    // create 1000 objects
    std::cout << "\ncreating objects :";
    for (int i = 0; i < 100000; ++i) {
      if (i%1000 == 0) {
        std::cout << " " << i;
        std::cout.flush();
      }
      object *o = ostore_.create("SIMPLE_OBJECT");
      
      ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
      
      SimpleObject *a = dynamic_cast<SimpleObject*>(o);
      
      ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
      
      simple_ptr simple = ostore_.insert(a);
      
      //ostore_.remove(simple);
    }
    std::cout << " done\n";
    
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

private:
  object_store ostore_;
};

bool test_1(object_store &ostore);
bool test_2(object_store &ostore);
bool test_3(object_store &ostore);
bool test_4(object_store &ostore);

int
main(int /*argc*/, char */*argv*/[])
{
  //test_suite::instance().register_unit("prototype", new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit("objects", new ObjectStoreTestUnit());
	
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

bool test_1(object_store &ostore)
{
  ostore.insert(new Album);
  ostore.insert(new Album);
  ostore.insert(new Track);
  ostore.insert(new AudioTrack);
  ostore.insert(new VideoTrack);
  ostore.insert(new Artist);
  ostore.insert(new VideoTrack);
  // show object hierarchy


  Track *track = NULL;
  for (int i = 0; i< 5; ++i) {
    track = new Track;
    if (!track) {
      cout << "couldn't create object of type 'TRACK'\n";
      return false;
    }
    ostore.insert(track);
  }
  Artist *artist = NULL;
  for (int i = 0; i< 5; ++i) {
    artist = new Artist;
    if (!artist) {
      cout << "couldn't create object of type 'TRACK'\n";
      return false;
    }
    ostore.insert(artist);
  }
  for (int i = 0; i< 5; ++i) {
    track = new Track;
    if (!track) {
      cout << "couldn't create object of type 'TRACK'\n";
      return false;
    }
    ostore.insert(track);
  }
  // show object hierarchy
  ostore.dump_prototypes(cout);

  ostore.dump_objects(cout);

  {
    object_view<Track> trackview(ostore, true);

    object_view<Track>::const_iterator first = trackview.begin();
    object_view<Track>::const_iterator last = trackview.end();

    cout << "Show all tracks (excluding siblings):\n";
    while (first != last) {
      object_ptr<Track> optr = (*first++);
      cout << "Track [" << *optr.get() << "]\n";
    }
    
    trackview.skip_siblings(false);
    first = trackview.begin();
    last = trackview.end();
    cout << "Show only tracks (including siblings):\n";
    while (first != last) {
      object_ptr<Track> optr = (*first++);
      cout << "Track [" << *optr.get() << "]\n";
    }
    cout << "Removing first track.\n"; 
    typedef object_ptr<Track> track_ptr;
    track_ptr t = trackview.begin().optr();

    ostore.remove(t);

    first = trackview.begin();
    last = trackview.end();
    cout << "Show only tracks (including siblings):\n";
    while (first != last) {
      object_ptr<Track> optr = (*first++);
      cout << "Track [" << *optr.get() << "]\n";
    }

    trackview.skip_siblings(true);
    cout << "Removing last track.\n"; 
    typedef object_ptr<Track> track_ptr;
    t = (--trackview.end()).optr();

    ostore.remove(t);

    ostore.dump_objects(std::cout);
    ostore.dump_prototypes(std::cout);
  }
  {
    object_view<VideoTrack> trackview(ostore);
    object_view<VideoTrack>::const_iterator first = trackview.begin();
    object_view<VideoTrack>::const_iterator last = trackview.end();

    cout << "Show all video tracks:\n";
    while (first != last) {
      object_ptr<VideoTrack> optr = (*first++);
      cout << "VideoTrack [" << *optr.get() << "]\n";
    }
  }
  {
    object_view<AudioTrack> atrackview(ostore);
    object_view<AudioTrack>::const_iterator first = atrackview.begin();
    object_view<AudioTrack>::const_iterator last = atrackview.end();

    cout << "Show all audio tracks:\n";
    while (first != last) {
      object_ptr<AudioTrack> optr = (*first++);
      cout << "AudioTrack [" << *optr.get() << "]\n";
    }
  }
  {
    object_view<Artist> artistview(ostore);
    object_view<Artist>::const_iterator first = artistview.begin();
    object_view<Artist>::const_iterator last = artistview.end();

    cout << "Show all artists:\n";
    while (first != last) {
      object_ptr<Artist> optr = (*first++);
      cout << "Artist [" << *optr.get() << "]\n";
    }
  }
  /**/
  /*
  typedef object_view<Track> TrackView;
  const TrackView track_view(ostore);
  
  if (track_view.empty()) {
    cout << "no tracks!\n";
  } else {
    cout << "tracks available!\n";
    TrackView::const_iterator first = track_view.begin();
    TrackView::const_iterator last = track_view.end();
    while (first != last) {
      cout << "Track #" << (*first++)->id() << "\n";
    }
    object_ptr<Track> track1 = track_view.begin().optr();
    object_ptr<Track> track2 = track1;
    track1->delete_object();
  }
  
  object_view<Album> album_view(ostore);
  
  if (album_view.empty()) {
    cout << "no albums!\n";
  } else {
    cout << "albums available!\n";
  }
  */
  return true;
}

bool test_2(object_store &ostore)
{
  typedef object_list<Item> ItemList;

  ItemList ilist;
  ostore.insert(ilist);

  ilist.push_back(new Item("Schrank"));
  ilist.push_back(new Item("Tisch"));
  ilist.push_back(new Item("Stuhl"));
  ilist.push_back(new Item("Bett"));
  ilist.push_front(new Item("Teppich"));

  ItemList::const_iterator first = ilist.begin();
  ItemList::const_iterator last = ilist.end();

  while (first != last) {
    cout << "item name: " << first->name() << "\n";
    ++first;
  }

  return true;
}

bool test_3(object_store &ostore)
{
  typedef object_ptr<Artist> artist_ptr;
  typedef object_ptr<Track> track_ptr;
  typedef object_ptr<Album> album_ptr;
  artist_ptr artist = ostore.insert(new Artist("Chris Huelsbeck"));
  album_ptr album = ostore.insert(new Album("symphonic shades", artist));
  
  track_ptr track = ostore.insert(new Track(1, "Grand Monster Slam (Opening Fanfare)", 165));
  album->add(track);
  track = ostore.insert(new Track(2, "X-Out (Main Theme)", 311));
  album->add(track);
  track = ostore.insert(new Track(3, "Jim Power in Mutant Planet (Main Theme)", 337));
  album->add(track);
  track = ostore.insert(new Track(4, "Tower of Babel", 279));
  album->add(track);
  track = ostore.insert(new Track(5, "Turrican 3 - Payment Day (Piano Suite)", 318));
  album->add(track);
  track = ostore.insert(new Track(6, "Gem'X (Main Theme)", 220));
  album->add(track);
  track = ostore.insert(new Track(7, "Apidya II (Suite)", 325));
  album->add(track);
  track = ostore.insert(new Track(8, "R-Type (Main Theme)", 318));
  album->add(track);
  track = ostore.insert(new Track(9, "Licht am Ende des Tunnels (Suite)", 309));
  album->add(track);
  track = ostore.insert(new Track(10, "The Great Giana Sisters (Suite)", 330));
  album->add(track);
  track = ostore.insert(new Track(11, "Rony Barrak-Solo", 305));
  album->add(track);
  track = ostore.insert(new Track(12, "Tunnel B1 (Suite)", 298));
  album->add(track);
  track = ostore.insert(new Track(13, "Symphonic Shades", 288));
  album->add(track);
  track = ostore.insert(new Track(14, "Karawane der Elefanten", 251));
  album->add(track);
  track = ostore.insert(new Track(15, "Turrican II - The Final Fight (Renderings: Main Theme)", 552));
  album->add(track);
  
  object_view<Track> trackview(ostore, true);

  object_view<Track>::const_iterator first = trackview.begin();
  object_view<Track>::const_iterator last = trackview.end();

  cout << "Show all tracks (excluding siblings):\n";
  while (first != last) {
    object_ptr<Track> optr = (*first++);
    cout << "Track: " << optr->title() << "\n";
  }

  Album::const_iterator afirst = album->begin();
  Album::const_iterator alast = album->end();

  typedef object_ref<Track> track_ref;

  cout << "Show all tracks of album [" << album->name() << "]:\n";
  while (afirst != alast) {
    track_ref tref = (*afirst++)->oref();
    cout << "Track: " << tref->title() << " (Artist: " << tref->artist()->name() << ")\n";
  }
  return true;
}

bool test_4(object_store &/*ostore*/)
{
  Track *track = new Track(1, "Hallo Welt", 300);
  
  object_serializer serializer;
  
  serializer.serialize(track);
  
  delete track;
  
  track = new Track();
  
  serializer.deserialize(track);

  cout << "track title: " << track->title() << "\n";

  return true;
}
