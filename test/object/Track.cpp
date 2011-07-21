#include "Track.hpp"

#include "object/object_atomizer.hpp"

Track::Track()
  : number_(0)
  , duration_(0)
{}

Track::Track(int numb, const std::string &t, unsigned long d)
  : number_(numb)
  , title_(t)
  , duration_(d)
{}

Track::Track(int numb, const std::string &t, unsigned long d, const object_ptr<Artist> &a)
  : number_(numb)
  , title_(t)
  , duration_(d)
  , artist_(a)
{}

Track::Track(int numb, const std::string &t, unsigned long d, const object_ptr<Artist> &a, const object_ptr<Album> &am)
  : number_(numb)
  , title_(t)
  , duration_(d)
  , artist_(a)
  , album_(am)
{}

Track::~Track()
{}

void Track::read_from(object_atomizer *reader)
{
	object::read_from(reader);
  reader->read_int("number", number_);
  reader->read_string("title", title_);
  reader->read_long("duration", duration_);
  reader->read_object("artist", artist_);
  reader->read_object("album", album_);
}

void Track::write_to(object_atomizer *writer) const
{
	object::write_to(writer);
  writer->write_int("number", number_);
  writer->write_string("title", title_);
  writer->write_long("duration", duration_);
  writer->write_object("artist", artist_);
  writer->write_object("album", album_);
}

void Track::number(int numb)
{
  number_ = numb;
}

int Track::number() const
{
  return number_;
}

void Track::title(const std::string &t)
{
  title_ = t;
}

std::string Track::title() const
{
	return title_;
}

void Track::duration(long d)
{
  duration_ = d;
}

long Track::duration() const
{
	return duration_;
}

void Track::artist(const object_ref<Artist> &a)
{
  mark_modified();
  artist_ = a;
}

object_ref<Artist> Track::artist() const
{
  return artist_;
}

void Track::album(const object_ref<Album> &a)
{
  mark_modified();
  album_ = a;
}

object_ref<Album> Track::album() const
{
  return album_;
}
