#ifndef TRACKALBUMRELATION_HPP
#define TRACKALBUMRELATION_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

class Album;
class Track;

class TrackAlbumRelation : public oos::object
{
public:
  TrackAlbumRelation();
  TrackAlbumRelation(int index, const oos::object_ref<Track> &track, const oos::object_ref<Album> &album);
  virtual ~TrackAlbumRelation();

  virtual void read_from(oos::object_atomizer *oa);
  virtual void write_to(oos::object_atomizer *oa) const;

  int index() const;
  void index(int i);

  oos::object_ref<Track> track() const;
  void track(const oos::object_ref<Track> &t);

  oos::object_ref<Album> album() const;
  void album(const oos::object_ref<Album> &a);

private:
  int track_index_;
  oos::object_ref<Track> track_;
  oos::object_ref<Album> album_;
};

#endif /* TRACKALBUMRELATION_HPP */
