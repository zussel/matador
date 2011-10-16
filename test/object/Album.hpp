#ifndef ALBUM_HPP
#define ALBUM_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"

#include <string>

class Track;
class Artist;

namespace oos {
  class object_atomizer;
}

class Album : public oos::object
{
public:
  Album();
  Album(const std::string &n);
  Album(const std::string &n, const oos::object_ptr<Artist> &a);
	virtual ~Album();

	virtual void read_from(oos::object_atomizer *reader);
	virtual void write_to(oos::object_atomizer *writer) const;

	void name(const std::string &n);
	std::string name() const;
	
  void artist(const oos::object_ptr<Artist> &a);
  oos::object_ref<Artist> artist() const;

	typedef oos::linked_object_ref_list<Track> TrackList;
//	typedef oos::object_list<oos::object_ref_list_node<Track> > TrackList;

  void add(Track *track, bool overide_artist = true);
  void add(oos::object_ref<Track> track, bool overide_artist = true);
  oos::object_ref<Track> find(const std::string &name) const;

  typedef TrackList::iterator iterator;
	typedef TrackList::const_iterator const_iterator;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;
	
  bool empty() const;

/*
	void push_front(const ObjectPtr<Track> &track);
	void push_back(const ObjectPtr<Track> &track);
*/
private:
	std::string name_;
  oos::object_ref<Artist> artist_;
	TrackList track_list_;
};

#endif /* ALBUM_HPP */
