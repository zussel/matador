#ifndef ALBUM_HPP
#define ALBUM_HPP

#include "object/object.hpp"
#include "object/object_vector.hpp"

#include <string>

class Track;
class Artist;

namespace oos {
  class object_atomizer;
}

class Album : public oos::object
{
public:
  typedef oos::object_ref<Album> self_ref;
  typedef oos::object_ref<Track> track_ref;
  typedef oos::object_vector<Album, track_ref> track_album_vector_t;
  typedef track_album_vector_t::item_type item_type;
  typedef track_album_vector_t::value_type value_type;
  typedef track_album_vector_t::iterator iterator;
	typedef track_album_vector_t::const_iterator const_iterator;
  typedef track_album_vector_t::size_type size_type;
  
  Album();
  Album(const std::string &n);
  Album(const std::string &n, int year, const oos::object_ptr<Artist> &a);
	virtual ~Album();

//	virtual void read_from(oos::object_reader *reader);
//	virtual void write_to(oos::object_writer *writer) const;

  virtual void deserialize(object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("name", name_);
    deserializer.read("year", year_);
    deserializer.read("artist", artist_);
    deserializer.read("track_album_rel", track_album_vector_);
  }
  virtual void serialize(object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("name", name_);
    serializer.write("year", year_);
    serializer.write("artist", artist_);
    serializer.write("track_album_rel", track_album_vector_);
  }

	void name(const std::string &n);
	std::string name() const;
	
	void year(int y);
	int year() const;
	
  void artist(const oos::object_ptr<Artist> &a);
  oos::object_ref<Artist> artist() const;

  void add(int index, const track_ref &track, bool overide_artist = true);
  const_iterator find(const std::string &title) const;

  track_ref operator[](track_album_vector_t::size_type n);

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;
	
  bool empty() const;
  size_t size() const;

private:
	std::string name_;
  int year_;
  oos::object_ref<Artist> artist_;
	track_album_vector_t track_album_vector_;
};

#endif /* ALBUM_HPP */
