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
  typedef track_album_vector_t::iterator iterator;
	typedef track_album_vector_t::const_iterator const_iterator;
  
  Album();
  Album(const std::string &n);
  Album(const std::string &n, int year, const oos::object_ptr<Artist> &a);
	virtual ~Album();

	virtual void read_from(oos::object_atomizer *reader);
	virtual void write_to(oos::object_atomizer *writer) const;

	void name(const std::string &n);
	std::string name() const;
	
	void year(int y);
	int year() const;
	
  void artist(const oos::object_ptr<Artist> &a);
  oos::object_ref<Artist> artist() const;

  void add(int index, const oos::object_ref<Track> &track, bool overide_artist = true);
  oos::object_ref<Track> find(const std::string &name) const;

  oos::object_ref<Track> operator[](track_album_vector_t::size_type n);

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
