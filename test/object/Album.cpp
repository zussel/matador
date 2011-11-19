#include "Album.hpp"
#include "Track.hpp"
#include "object/object_atomizer.hpp"

using std::string;
using namespace oos;

Album::Album()
  : track_list_(this, "album")
{}

Album::Album(const std::string &n)
  : name_(n)
  , track_list_(this, "album")
{}

Album::Album(const std::string &n, const object_ptr<Artist> &a)
  : name_(n)
  , artist_(a)
  , track_list_(this, "album")
{}

Album::~Album()
{}

void Album::read_from(object_atomizer *reader)
{
	object::read_from(reader);
	reader->read_string("name", name_);
  reader->read_object("artist", artist_);
	reader->read_object_list("track_list", track_list_);
}

void Album::write_to(object_atomizer *writer) const
{
	object::write_to(writer);
	writer->write_string("name", name_);
  writer->write_object("artist", artist_);
	writer->write_object_list("track_list", track_list_);
}

void Album::name(const std::string &n)
{
	mark_modified();
	name_ = n;
}

string Album::name() const
{
	return name_;
}

void Album::artist(const object_ptr<Artist> &a)
{
  mark_modified();
  artist_ = a;
}

object_ref<Artist> Album::artist() const
{
  return artist_;
}

void Album::add(Track *track, bool overide_artist)
{
  if (!ostore()) {
    // throw object_exception();
  } else {
    object_ref<Track> track_ref = ostore()->insert(track);
    this->add(track_ref, overide_artist);
  }
}

void Album::add(object_ref<Track> track, bool override_artist)
{
//  TrackList::t_list_node *node = new TrackList::t_list_node(track);
  track_list_.push_back(track, self_ref(this));
  if (override_artist) {
    track->artist(artist_);
  }
}

object_ref<Track> Album::find(const std::string &/*name*/) const
{
  return object_ref<Track>();
}

Album::iterator Album::begin()
{
	return track_list_.begin();
}

Album::iterator Album::end()
{
	return track_list_.end();
}

Album::const_iterator Album::begin() const
{
	return track_list_.begin();
}

Album::const_iterator Album::end() const
{
	return track_list_.end();
}

bool Album::empty() const
{
	return track_list_.empty();
}

size_t Album::size() const
{
  return track_list_.size();
}

/*
void Album::push_front(const ObjectPtr<Track> &track)
{
	track_list_.push_front(track);
}

void Album::push_back(const ObjectPtr<Track> &track)
{
	track_list_.push_back(track);
}
*/
