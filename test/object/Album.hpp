#ifndef ALBUM_HPP
#define ALBUM_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"

#include <string>

class Track;
class Artist;

class Album : public object
{
public:
  Album();
  Album(const std::string &n);
  Album(const std::string &n, const object_ptr<Artist> &a);
	virtual ~Album();

	virtual void read_from(object_atomizer *reader);
	virtual void write_to(object_atomizer *writer) const;

	void name(const std::string &n);
	std::string name() const;
	
  void artist(const object_ptr<Artist> &a);
  object_ref<Artist> artist() const;

	typedef object_list<object_ref_list_node<Track> > TrackList;

  void add(object_ptr<Track> track, bool overide_artist = true);
  object_ref<Track> find(const std::string &name) const;

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
  object_ref<Artist> artist_;
	TrackList track_list_;
};

#endif /* ALBUM_HPP */
