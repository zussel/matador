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
  Track(int numb, const std::string &t, unsigned long d);
  Track(int numb, const std::string &t, unsigned long d, const oos::object_ptr<Artist> &a);
  Track(int numb, const std::string &t, unsigned long d, const oos::object_ptr<Artist> &a, const oos::object_ptr<Album> &am);
	virtual ~Track();
	
	virtual void read_from(oos::object_atomizer *reader);
  virtual void write_to(oos::object_atomizer *writer) const;

  void number(int numb);
  int number() const;

  void title(const std::string &t);
	std::string title() const;

  void duration(long d);
	long duration() const;

  void artist(const oos::object_ref<Artist> &a);
  oos::object_ref<Artist> artist() const;

  void album(const oos::object_ref<Album> &a);
  oos::object_ref<Album> album() const;

private:
  int number_;
	std::string title_;
	long duration_;
  oos::object_ref<Artist> artist_;
  oos::object_ref<Album> album_;
};

class MediaTrack : public Track
{
public:
  MediaTrack() {}
  virtual ~MediaTrack() {}
};

class AudioTrack : public MediaTrack
{
public:
  AudioTrack() {}
  virtual ~AudioTrack() {}
};

class VideoTrack : public MediaTrack
{
public:
  VideoTrack() {}
  virtual ~VideoTrack() {}
};

#endif /* TRACK_HPP */
