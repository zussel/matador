#ifndef TRACK_HPP
#define TRACK_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

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
	
//  virtual void read_from(oos::object_reader *reader);
//  virtual void write_to(oos::object_writer *writer) const;

  virtual void deserialize(object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("title", title_);
    deserializer.read("duration", duration_);
    deserializer.read("artist", artist_);
  }
  virtual void serialize(object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("title", title_);
    serializer.write("duration", duration_);
    serializer.write("artist", artist_);
  }

  void title(const std::string &t);
	std::string title() const;

  void duration(long d);
	long duration() const;

  void artist(const oos::object_ref<Artist> &a);
  oos::object_ref<Artist> artist() const;

private:
  oos::varchar<64> title_;
	long duration_;
  oos::object_ref<Artist> artist_;
};

#endif /* TRACK_HPP */
