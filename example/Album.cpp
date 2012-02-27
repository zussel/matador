#include "Album.hpp"
#include "Track.hpp"
#include "object/object_atomizer.hpp"

using std::string;
using namespace oos;

Album::Album()
  : track_vector_(this, "album", "number")
{}

Album::Album(const std::string &n)
  : name_(n)
  , track_vector_(this, "album", "number")
{}

Album::Album(const std::string &n, const object_ptr<Artist> &a)
  : name_(n)
  , artist_(a)
  , track_vector_(this, "album", "number")
{}

Album::~Album()
{}

void Album::read_from(object_atomizer *reader)
{
	object::read_from(reader);
	reader->read_string("name", name_);
	reader->read_int("year", year_);
  reader->read_object("artist", artist_);
	reader->read_object_vector("track_vector", track_vector_);
}

void Album::write_to(object_atomizer *writer) const
{
	object::write_to(writer);
	writer->write_string("name", name_);
	writer->write_int("year", year_);
  writer->write_object("artist", artist_);
	writer->write_object_vector("track_vector", track_vector_);
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

void Album::year(int y)
{
  mark_modified();
  year_ = y;
}

int Album::year() const
{
  return year_;
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
  track_vector_.push_back(track);
  if (override_artist) {
    track->artist(artist_);
  }
}

object_ref<Track> Album::find(const std::string &/*name*/) const
{
  return object_ref<Track>();
}

oos::object_ptr<Track> Album::operator[](track_vector_t::size_type n)
{
  return track_vector_[n];
}

Album::iterator Album::begin()
{
	return track_vector_.begin();
}

Album::iterator Album::end()
{
	return track_vector_.end();
}

Album::const_iterator Album::begin() const
{
	return track_vector_.begin();
}

Album::const_iterator Album::end() const
{
	return track_vector_.end();
}

bool Album::empty() const
{
	return track_vector_.empty();
}

size_t Album::size() const
{
  return track_vector_.size();
}
