#ifndef TRACK_HPP
#define TRACK_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include "Artist.hpp"
#include "Album.hpp"

#include <string>

namespace oos {
  class object_atomizer;
}

class Track : public oos::object
{
public:
	Track();
  Track(const std::string &t, unsigned long d, const oos::object_ref<Artist> &a);
	virtual ~Track();
	
	virtual void read_from(oos::object_atomizer *reader);
  virtual void write_to(oos::object_atomizer *writer) const;

  void title(const std::string &t);
	std::string title() const;

  void duration(long d);
	long duration() const;

  void artist(const oos::object_ref<Artist> &a);
  oos::object_ref<Artist> artist() const;

private:
	std::string title_;
	long duration_;
  oos::object_ref<Artist> artist_;
};

#endif /* TRACK_HPP */
