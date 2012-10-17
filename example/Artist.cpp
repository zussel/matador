#include "Artist.hpp"
#include "Album.hpp"

#include "object/object_atomizer.hpp"
#include "object/object_expression.hpp"

using namespace oos;

Artist::Artist()
  : album_artist_list_(this)
{}

Artist::Artist(const std::string &n)
  : name_(n)
  , album_artist_list_(this)
{}

Artist::~Artist()
{}
/*
void Artist::read_from(object_reader *reader)
{
	object::read_from(reader);
  reader->read("name", name_);
  reader->read("album_artist_rel", album_artist_list_);
}

void Artist::write_to(object_writer *writer) const
{
	object::write_to(writer);
  writer->write("name", name_);
  writer->write("album_artist_rel", album_artist_list_);
}
*/
std::string Artist::name() const
{
  return name_.str();
}

void Artist::name(const std::string &n)
{
  modify(name_, n);
}

void Artist::add(const Artist::album_ref &album)
{
  if (find(album->name()) == album_artist_list_.end()) {
    album_artist_list_.push_back(album);
  }
}

Artist::const_iterator Artist::find(const std::string &name) const
{
  typedef variable<value_type, item_type> item_var_t;

  variable<std::string, Album, item_var_t> x(&Album::name, item_var_t(&item_type::value));

  return std::find_if(album_artist_list_.begin(), album_artist_list_.end(), x == name);
}

Artist::iterator Artist::begin()
{
  return album_artist_list_.begin();
}

Artist::iterator Artist::end()
{
  return album_artist_list_.end();
}

Artist::const_iterator Artist::begin() const
{
  return album_artist_list_.begin();
}

Artist::const_iterator Artist::end() const
{
  return album_artist_list_.end();
}

bool Artist::empty() const
{
  return album_artist_list_.empty();
}

size_t Artist::size() const
{
  return album_artist_list_.size();
}

std::ostream& operator <<(std::ostream &os, const Artist &a)
{
  os << "Artist [" << a.name() << "]";
  return os;
}
