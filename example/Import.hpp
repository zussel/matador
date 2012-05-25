#ifndef IMPORT_HPP
#define IMPORT_HPP

#include "Command.hpp"

class Import : public Command
{
private:
  typedef oos::object_ptr<Artist> artist_ptr;
  typedef oos::object_ptr<Album> album_ptr;
  typedef oos::object_ptr<Track> track_ptr;

public:
  Import();
  virtual ~Import();

  virtual void execute(oos::session &db, const std::vector<std::string> &args);

private:
  void parse(oos::object_store &ostore, const std::string &line, size_t pos);
  void parse(oos::object_store &ostore, const artist_ptr &artist, const std::string &line, size_t pos);
  void parse(oos::object_store &ostore, const album_ptr &album, const std::string &line, size_t pos);

private:
  artist_ptr artist_;
  album_ptr album_;
  track_ptr track_;
};

#endif /* IMPORT_HPP */
