#ifndef TRACK_HPP
#define TRACK_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include "Artist.hpp"
#include "Album.hpp"

#include <string>

class Track : public object
{
public:
	Track();
  Track(int numb, const std::string &t, unsigned long d);
  Track(int numb, const std::string &t, unsigned long d, const object_ptr<Artist> &a);
  Track(int numb, const std::string &t, unsigned long d, const object_ptr<Artist> &a, const object_ptr<Album> &am);
	virtual ~Track();
	
	virtual void read_from(object_atomizer *reader);
  virtual void write_to(object_atomizer *writer) const;

  void number(int numb);
  int number() const;

  void title(const std::string &t);
	std::string title() const;

  void duration(long d);
	long duration() const;

  void artist(const object_ref<Artist> &a);
  object_ref<Artist> artist() const;

  void album(const object_ref<Album> &a);
  object_ref<Album> album() const;

private:
  int number_;
	std::string title_;
	long duration_;
  object_ref<Artist> artist_;
  object_ref<Album> album_;
};

#endif /* TRACK_HPP */
