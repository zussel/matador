#include "Artist.hpp"

#include "object/object_atomizer.hpp"

using namespace oos;

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

void Artist::write_to(object_atomizer *writer) const
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
  mark_modified();
  name_ = n;
}

std::ostream& operator <<(std::ostream &os, const Artist &a)
{
  os << "Artist [" << a.name() << "]";
  return os;
}
