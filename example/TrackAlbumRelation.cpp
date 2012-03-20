#include "TrackAlbumRelation.hpp"

#include "Album.hpp"
#include "Track.hpp"

TrackAlbumRelation::TrackAlbumRelation()
  : track_index_(0)
{}

TrackAlbumRelation::TrackAlbumRelation(int index, const oos::object_ref<Track> &track, const oos::object_ref<Album> &album)
  : track_index_(index)
  , track_(track)
  , album_(album)
{}

TrackAlbumRelation::~TrackAlbumRelation()
{}

void TrackAlbumRelation::read_from(oos::object_atomizer *oa)
{
	object::read_from(oa);
  oa->read_int("track_index", track_index_);
  oa->read_object("track", track_);
  oa->read_object("album", album_);
}

void TrackAlbumRelation::write_to(oos::object_atomizer *oa) const
{
	object::write_to(oa);
  oa->write_int("track_index", track_index_);
  oa->write_object("track", track_);
  oa->write_object("album", album_);
}

int TrackAlbumRelation::index() const
{
  return track_index_;
}

void TrackAlbumRelation::index(int i)
{
  modify(track_index_, i);
}

oos::object_ref<Track> TrackAlbumRelation::track() const
{
  return track_;
}

void TrackAlbumRelation::track(const oos::object_ref<Track> &t)
{
  modify(track_, t);
}

oos::object_ref<Album> TrackAlbumRelation::album() const
{
  return album_;
}

void TrackAlbumRelation::album(const oos::object_ref<Album> &a)
{
  modify(album_, a);
}
