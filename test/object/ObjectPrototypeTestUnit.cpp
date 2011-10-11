#include "ObjectPrototypeTestUnit.hpp"

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"

#include "Track.hpp"
#include "Album.hpp"
#include "Artist.hpp"

using namespace oos;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("ObjectStore Prototype Test Unit")
{
  add_test("one", std::tr1::bind(&ObjectPrototypeTestUnit::one_prototype, this), "one prototype");
  add_test("hierarchy", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_hierachy, this), "prototype hierarchy");
}

ObjectPrototypeTestUnit::~ObjectPrototypeTestUnit()
{}

void
ObjectPrototypeTestUnit::initialize()
{}

void
ObjectPrototypeTestUnit::finalize()
{}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;
//  ostore.insert_prototype(new object_producer<Artist>, "ARTIST");
  ostore.insert_prototype<Artist>("ARTIST");
  
  object *o = ostore.create("ARTIST");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <Artist>");
  
  Artist *a = dynamic_cast<Artist*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to Artist");
  
  delete a;
  
  ostore.remove_prototype("ARTIST");
  
  o = ostore.create("ARTIST");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.insert_prototype<Track>("TRACK");
  ostore.insert_prototype<MediaTrack>("MEDIATRACK", "TRACK");
  ostore.insert_prototype<AudioTrack>("AUDIOTRACK", "TRACK");
  ostore.insert_prototype<VideoTrack>("VIDEOTRACK", "TRACK");

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
