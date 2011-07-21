#include "Artist.hpp"

#include "object/object_atomizer.hpp"

Artist::Artist()
{}

Artist::Artist(const std::string &n)
  : name_(n)
{}

Artist::~Artist()
{}

void Artist::read_from(object_atomizer *reader)
{
	object::read_from(reader);
  reader->read_string("name", name_);
}

void Artist::write_to(object_atomizer *writer)
{
	object::write_to(writer);
  writer->write_string("name", name_);
}

std::string Artist::name() const
{
  return name_;
}

void Artist::name(const std::string &n)
{
  name_ = n;
}
