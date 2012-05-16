#include "Artist.hpp"
#include "Album.hpp"
#include "Track.hpp"

#include "object/object_store.hpp"
#include "object/object_view.hpp"
#include "object/object_expression.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <errno.h>
#include <limits.h>

using namespace oos;

bool read_music_data(session &db, const char *fname);
bool line_is(const char *token, const std::string &line, size_t &pos);
object_ptr<Artist> parse_artist(object_store &ostore, const std::string &line, size_t pos);
object_ptr<Album> parse_album(object_store &ostore, const object_ptr<Artist> &artist, const std::string &line, size_t pos);
object_ptr<Track> parse_track(object_store &ostore, const object_ptr<Album> &album, const std::string &line, size_t pos);
bool begin_data(const std::string &line, size_t &pos);
bool extract_string(const std::string &line, std::string &val);
bool extract_integer(const std::string &line, int &val);
bool extract_time(const std::string &line, time_t &val);

size_t split(const std::string &str, char delim, std::vector<std::string> &values);

int
main(int argc, char *argv[])
{
  // create object store
  object_store ostore;

  // setup object hierarchy
  ostore.insert_prototype<Artist>("artist");
  ostore.insert_prototype<Track>("track");
  ostore.insert_prototype<Album>("album");
  ostore.insert_prototype<Album::track_album_vector_t::item_type>("track_album_rel");

  // create and open db
  session db(ostore, "sqlite://mdb.sqlite");

  // create db
  db.create();
  // load db
  db.load();

  /*********************
   *
   * setup command server
   * with following commands:
   * - import
   * - export
   * - list [artist,album,track]
   * - add [artist,album,track]
   * - delete [artist,album,track]
   * - update [artist,album,track]
   *
   *********************/

  // 
  // run command server
  /*
  server app(ostore);
  server.insert_command<import_command>("import");
  server.insert_command<export_command>("export");
  server.insert_command<insert_command>("insert");
  server.insert_command<update_command>("update");
  server.insert_command<delete_command>("delete");
  server.insert_command<select_command>("select");
  app.run();
  */

  // read in data from file
  read_music_data(db, "musicdata.txt");

  // close db
  return 0;
}

bool
read_music_data(session &db, const char *fname)
{
  std::ifstream fin(fname);

  if (!fin) {
    return false;
  }

  enum data_type_t {
    DATA_NONE = 0,
    DATA_ARTIST,
    DATA_ALBUM,
    DATA_TRACK
  };

  data_type_t data_type = DATA_NONE;

  object_ptr<Artist> artist;
  object_ptr<Album> album;
  object_ptr<Track> track;

  transaction tr(db);
  try {
    tr.begin();

    std::string line;
    while (std::getline(fin, line)) {
      size_t pos = 0;
      switch (data_type) {
        case DATA_NONE:
          if (line_is("ARTIST", line, pos)) {
            artist = parse_artist(db.ostore(), line, pos);
            data_type = DATA_ARTIST;
          } else {
            throw std::logic_error("line isn't artist");
          }
          break;
        case DATA_ARTIST:
          if (line_is("ALBUM", line, pos)) {
            album = parse_album(db.ostore(), artist, line, pos);
            data_type = DATA_TRACK;
          } else if (line_is("ARTISTEND", line, pos)) {
            // end artist section
            data_type = DATA_NONE;
          } else {
            throw std::logic_error("line isn't album");
          }
          break;
        case DATA_ALBUM:
        case DATA_TRACK:
          if (line_is("TRACK", line, pos)) {
            track = parse_track(db.ostore(), album, line, pos);
            data_type = DATA_TRACK;
          } else if (line_is("ALBUMEND", line, pos)) {
            // end artist section
            data_type = DATA_ARTIST;
          } else {
            throw std::logic_error("line isn't track");
          }
          break;
        default:
          throw std::runtime_error("unknown data type");
          break;
      }
    }

    tr.commit();
  } catch (std::exception &ex) {
    std::string err(ex.what());
    std::cout << "caugt exception: " << ex.what() << "\n";
    tr.rollback();
    return false;
  }

  return true;
}

bool line_is(const char *token, const std::string &line, size_t &pos)
{
  pos = line.find(' ');
  return line.substr(0, pos) == token;
}

object_ptr<Artist> parse_artist(object_store &ostore, const std::string &line, size_t pos)
{
  // find begin of data set
  if (!begin_data(line, pos)) {
    throw std::logic_error("parse artist: couldn't find opening data bracket");
  }

  std::vector<std::string> values;
  if (split(line.substr(pos), '|', values) < 1) {
    throw std::logic_error("invalid number of artist arguments");
  }

  std::string name;
  if (!extract_string(values[0], name)) {
    throw std::logic_error("couldn't extract name of artist from value [" + values[0] + "]");
  }

  object_view<Artist> oview(ostore);
  variable<std::string, Artist> x(&Artist::name);
  if (oview.find_if(x == name) != oview.end()) {
//  if (oview.find_if(&Artist::name, name) != oview.end()) {
    // artist already exists
    std::cout << "artist [" << name << "] already exists!\n";
    return object_ptr<Artist>();
  } else {
    return ostore.insert(new Artist(name));
  }
}

object_ptr<Album> parse_album(object_store &ostore, const object_ptr<Artist> &artist, const std::string &line, size_t pos)
{
  // find begin of data set
  if (!begin_data(line, pos)) {
    throw std::logic_error("parse album: couldn't find opening data bracket");
  }

  std::vector<std::string> values;
  if (split(line.substr(pos), '|', values) < 2) {
    throw std::logic_error("invalid number of album arguments");
  }

  std::string name;
  if (!extract_string(values[0], name)) {
    throw std::logic_error("couldn't extract name of album from value [" + values[0] + "]");
  }
  int year = 0;
  if (!extract_integer(values[1], year)) {
    throw std::logic_error("couldn't extract year of album from value [" + values[1] + "]");
  }

  object_view<Album> oview(ostore);
  variable<std::string, Album> x(&Album::name);
  if (oview.find_if(x == name) != oview.end()) {
    // artist already exists
    std::cout << "album [" << name << "] already exists!\n";
    return object_ptr<Album>();
  } else {
    return ostore.insert(new Album(name, year, artist));
  }
}

object_ptr<Track> parse_track(object_store &ostore, const object_ptr<Album> &album, const std::string &line, size_t pos)
{
  // find begin of data set
  if (!begin_data(line, pos)) {
    throw std::logic_error("parse track: couldn't find opening data bracket");
  }

  std::vector<std::string> values;
  if (split(line.substr(++pos), '|', values) < 3) {
    throw std::logic_error("invalid number of track arguments");
  }

  int number = 0;
  if (!extract_integer(values[0], number)) {
    throw std::logic_error("couldn't extract number of track from value [" + values[0] + "]");
  }
  std::string name;
  if (!extract_string(values[1], name)) {
    throw std::logic_error("couldn't extract name of track from value [" + values[1] + "]");
  }
  time_t duration = 0;
  if (!extract_time(values[2], duration)) {
    throw std::logic_error("couldn't extract duration of track from value [" + values[2] + "]");
  }

  object_view<Track> oview(ostore);
  variable<std::string, Track> x(&Track::title);
  if (oview.find_if(x == name) != oview.end()) {
    // artist already exists
    std::cout << "track [" << name << "] already exists!\n";
    return object_ptr<Track>();
  } else {
    object_ptr<Track> track = ostore.insert(new Track(name, (long)duration, album->artist()));
    album->add(number, track);
    return track;
  }
}

bool begin_data(const std::string &line, size_t &pos)
{
  pos = line.find('[', pos);
  return pos != std::string::npos;
}

bool extract_string(const std::string &item, std::string &val)
{
  size_t pos = item.find('"');
  if (pos == std::string::npos) {
    return false;
  }
  size_t end = item.find('"', ++pos);
  if (end == std::string::npos) {
    return false;
  }
  val = item.substr(pos, end-pos);
  return true;
}

bool extract_integer(const std::string &item, int &val)
{
  char *ptr;
  long ret = strtol(item.c_str(), &ptr, 10);

  if ((ret == LONG_MAX || ret == LONG_MIN) && errno == ERANGE) {
    return false;
  }
  val = (int)ret;
  return true;
}

bool extract_time(const std::string &line, time_t &val)
{
  val = 300;
  return true;
}

size_t split(const std::string &str, char delim, std::vector<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}
