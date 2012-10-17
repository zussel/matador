#ifndef ARTIST_HPP
#define ARTIST_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"

#include "tools/varchar.hpp"

#include <string>

class Album;

namespace oos {
  class object_atomizer;
}

class Artist : public oos::object
{
public:
  typedef oos::object_ref<Album> album_ref;
  typedef oos::object_list<Artist, album_ref> album_artist_list_t;
  typedef album_artist_list_t::item_type item_type;
  typedef album_artist_list_t::value_type value_type;
  typedef album_artist_list_t::size_type size_type;
  typedef album_artist_list_t::iterator iterator;
	typedef album_artist_list_t::const_iterator const_iterator;

public:
  Artist();
  Artist(const std::string &n);
  virtual ~Artist();

//  void read_from(oos::object_reader *reader);
//  void write_to(oos::object_writer *writer) const;

  template < class S >
  void deserialize(S &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("name", name_);
    deserializer.read("album_artist_rel", album_artist_list_);
  }
  template < class S >
  void serialize(S &serializer)
  {
    oos::object::serialize(serializer);
    serializer.write("name", name_);
    serializer.write("album_artist_rel", album_artist_list_);
  }

  std::string name() const;
  void name(const std::string &n);

  void add(const album_ref &album);
  const_iterator find(const std::string &name) const;

  iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;
	
  bool empty() const;
  size_t size() const;

  friend std::ostream& operator <<(std::ostream &os, const Artist &a);

private:
  oos::varchar<64> name_;
  album_artist_list_t album_artist_list_;
};

#endif /* ARTIST_HPP */
