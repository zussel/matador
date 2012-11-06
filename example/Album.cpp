#include "Album.hpp"
#include "Track.hpp"

#include "object/object_expression.hpp"

using std::string;
using namespace oos;

Album::Album()
  : track_album_vector_(this)
{}

Album::Album(const std::string &n)
  : name_(n)
  , track_album_vector_(this)
{}

Album::Album(const std::string &n, int year, const object_ptr<Artist> &a)
  : name_(n)
  , year_(year)
  , artist_(a)
  , track_album_vector_(this)
{}

Album::~Album()
{}
/*
void Album::read_from(object_reader *reader)
{
	object::read_from(reader);
	reader->read("name", name_);
	reader->read("year", year_);
  reader->read("artist", artist_);
  reader->read("track_album_rel", track_album_vector_);
}

void Album::write_to(object_writer *writer) const
{
	object::write_to(writer);
	writer->write("name", name_);
	writer->write("year", year_);
  writer->write("artist", artist_);
  writer->write("track_album_rel", track_album_vector_);
}
*/
void Album::name(const std::string &n)
{
	modify(name_, n);
}

string Album::name() const
{
	return name_;
}

void Album::year(int y)
{
  modify(year_, y);
}

int Album::year() const
{
  return year_;
}

void Album::artist(const object_ptr<Artist> &a)
{
  modify(artist_, a);
}

object_ref<Artist> Album::artist() const
{
  return artist_;
}

void Album::add(int /*index*/, const track_ref &track, bool override_artist)
{
  track_album_vector_.push_back(track);
  if (override_artist) {
    track->artist(artist_);
  }
}

Album::const_iterator Album::find(const std::string &title) const
{
  variable<std::string> x(make_var(&item_type::value, &Track::title));

  return std::find_if(track_album_vector_.begin(), track_album_vector_.end(), x == title);
}

Album::track_ref Album::operator[](track_album_vector_t::size_type n)
{
  return track_album_vector_[n];
}

Album::iterator Album::begin()
{
	return track_album_vector_.begin();
}

Album::iterator Album::end()
{
	return track_album_vector_.end();
}

Album::const_iterator Album::begin() const
{
	return track_album_vector_.begin();
}

Album::const_iterator Album::end() const
{
	return track_album_vector_.end();
}

bool Album::empty() const
{
	return track_album_vector_.empty();
}

size_t Album::size() const
{
  return track_album_vector_.size();
}
