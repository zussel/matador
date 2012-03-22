#include "ComplexObjectStoreTestUnit.hpp"

#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/object_view.hpp"

#include "Track.hpp"
#include "Album.hpp"
#include "Artist.hpp"

#include <iostream>
#include <fstream>

using namespace oos;
using namespace std;

typedef linked_object_ref_list_node<Track> AlbumTrack;

ComplexObjectStoreTestUnit::ComplexObjectStoreTestUnit()
  : unit_test("complex objectstore")
{
  add_test("first", std::tr1::bind(&ComplexObjectStoreTestUnit::first, this), "first complex test");
  add_test("delete_object", std::tr1::bind(&ComplexObjectStoreTestUnit::delete_object, this), "delete object");
}

ComplexObjectStoreTestUnit::~ComplexObjectStoreTestUnit()
{
}

void
ComplexObjectStoreTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<Artist>, "ARTIST");
  ostore_.insert_prototype(new object_producer<Track>, "TRACK");
  ostore_.insert_prototype(new object_producer<Album>, "ALBUM");
  ostore_.insert_prototype(new object_producer<AlbumTrack>, "ALBUMTRACK");
  ostore_.insert_prototype(new object_producer<MediaTrack>, "MEDIATRACK", false, "TRACK");
  ostore_.insert_prototype(new object_producer<AudioTrack>, "AUDIOTRACK", false, "MEDIATRACK");
  ostore_.insert_prototype(new object_producer<VideoTrack>, "VIDEOTRACK", false, "MEDIATRACK");
}

void
ComplexObjectStoreTestUnit::finalize()
{
  ostore_.clear();
}

void
ComplexObjectStoreTestUnit::first()
{
  ostore_.insert(new Album);
  ostore_.insert(new Album);
  ostore_.insert(new Track);
  ostore_.insert(new AudioTrack);
  ostore_.insert(new VideoTrack);
  ostore_.insert(new Artist);
  ostore_.insert(new VideoTrack);
  // show object hierarchy
  ostore_.dump_objects(cout);


  Track *track = NULL;
  for (int i = 0; i< 5; ++i) {
    track = new Track;
    if (!track) {
      cout << "couldn't create object of type 'TRACK'\n";
      return;
    }
    ostore_.insert(track);
  }
  Artist *artist = NULL;
  for (int i = 0; i< 5; ++i) {
    artist = new Artist;
    if (!artist) {
      cout << "couldn't create object of type 'TRACK'\n";
      return;
    }
    ostore_.insert(artist);
  }
  for (int i = 0; i< 5; ++i) {
    track = new Track;
    if (!track) {
      cout << "couldn't create object of type 'TRACK'\n";
      return;
    }
    ostore_.insert(track);
  }
  // show object hierarchy
  ostore_.dump_prototypes(cout);

  ostore_.dump_objects(cout);

  {
    object_view<Track> trackview(ostore_, true);

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

    ostore_.remove(t);

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

    ostore_.remove(t);

    ostore_.dump_objects(std::cout);
    ostore_.dump_prototypes(std::cout);
  }
  {
    object_view<VideoTrack> trackview(ostore_);
    object_view<VideoTrack>::const_iterator first = trackview.begin();
    object_view<VideoTrack>::const_iterator last = trackview.end();

    cout << "Show all video tracks:\n";
    while (first != last) {
      object_ptr<VideoTrack> optr = (*first++);
      cout << "VideoTrack [" << *optr.get() << "]\n";
    }
  }
  {
    object_view<AudioTrack> atrackview(ostore_);
    object_view<AudioTrack>::const_iterator first = atrackview.begin();
    object_view<AudioTrack>::const_iterator last = atrackview.end();

    cout << "Show all audio tracks:\n";
    while (first != last) {
      object_ptr<AudioTrack> optr = (*first++);
      cout << "AudioTrack [" << *optr.get() << "]\n";
    }
  }
  {
    object_view<Artist> artistview(ostore_);
    object_view<Artist>::const_iterator first = artistview.begin();
    object_view<Artist>::const_iterator last = artistview.end();

    cout << "Show all artists:\n";
    while (first != last) {
      object_ptr<Artist> optr = (*first++);
      cout << "Artist [" << *optr.get() << "]\n";
    }
  }

  typedef object_view<Track> TrackView;
  const TrackView track_view(ostore_);
  
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
//    track1->delete_object();
  }
  
  object_view<Album> album_view(ostore_);
  
  if (album_view.empty()) {
    cout << "no albums!\n";
  } else {
    cout << "albums available!\n";
  }
}

void
ComplexObjectStoreTestUnit::delete_object()
{
  Artist *a = new Artist("AC/CD");
  
  typedef object_ptr<Artist> artist_ptr;
  
  artist_ptr artist = ostore_.insert(a);

  Track *t = new Track(1, "Thunderstruck", 360, artist);
  
  typedef object_ptr<Track> track_ptr;
  
  track_ptr track = ostore_.insert(t);
  
  cout << std::endl;
  cout << "artist ref count: " << artist.ref_count() << "\n";
  cout << "artist ptr count: " << artist.ptr_count() << "\n";
  cout << "track ref count: " << track.ref_count() << "\n";
  cout << "track ptr count: " << track.ptr_count() << "\n";
  
  typedef object_view<Artist> artist_view;
  artist_view aview(ostore_);

  artist_view::iterator afirst = aview.begin();
  artist_view::iterator alast = aview.begin();
  while (afirst != alast) {
    cout << "track title: " << (afirst++)->name() << "\n";
  }

  if (!ostore_.remove(artist)) {
    cout << "couldn't remove artist\n";
  } else {
    cout << "removed artist\n";
  }  

  cout << "artist view is" << (aview.empty() ? " " : " not ") << "empty\n";

  if (!ostore_.remove(track)) {
    cout << "couldn't remove track\n";
  } else {
    cout << "removed track\n";
  }  
  
  if (!ostore_.remove(artist)) {
    cout << "couldn't remove artist\n";
  } else {
    cout << "removed artist\n";
  }  

  cout << "artist view is" << (aview.empty() ? " " : " not ") << "empty\n";
}
