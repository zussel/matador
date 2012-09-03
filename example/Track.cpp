#include "Track.hpp"

#include "object/object_atomizer.hpp"

using namespace oos;

Track::Track()
  : duration_(0)
{}

Track::Track(const std::string &t, unsigned long d, const object_ref<Artist> &a)
  : title_(t)
  , duration_(d)
  , artist_(a)
{}

Track::~Track()
{}

void Track::read_from(object_reader *reader)
{
	object::read_from(reader);
  reader->read("title", title_);
  reader->read("duration", duration_);
  reader->read("artist", artist_);
}

void Track::write_to(object_writer *writer) const
{
	object::write_to(writer);
  writer->write("title", title_);
  writer->write("duration", duration_);
  writer->write("artist", artist_);
}

void Track::title(const std::string &t)
{
  modify(title_, t);
}

std::string Track::title() const
{
	return title_.str();
}

void Track::duration(long d)
{
  modify(duration_, d);
}

long Track::duration() const
{
	return duration_;
}

void Track::artist(const object_ref<Artist> &a)
{
  modify(artist_, a);
}

object_ref<Artist> Track::artist() const
{
  return artist_;
}
