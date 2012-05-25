#include "Artist.hpp"
#include "Album.hpp"
#include "Track.hpp"

#include "Import.hpp"

#include "object/object_store.hpp"
#include "object/object_view.hpp"
#include "object/object_expression.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"

#include "tools/string.hpp"

#include <vector>
#include <fstream>
#include <errno.h>
#include <limits.h>

using namespace oos;

Import::Import()
  : Command("import")
{}

Import::~Import()
{}

bool line_is(const char *token, const std::string &line, size_t &pos)
{
  pos = line.find(' ');
  return line.substr(0, pos) == token;
}

void Import::execute(session &db, const std::vector<std::string> &args)
{
  if (!db.is_open()) {
    throw std::logic_error("database isn't open");
  } else {
    if (args.empty()) {
      throw std::logic_error("missing file argument");
    }

    db.load();

    std::ifstream fin(args[0].c_str());

    if (!fin) {
      return throw std::logic_error("couldn't open file [" + args[0] + "]");
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
              parse(db.ostore(), line, pos);
              data_type = DATA_ARTIST;
            } else {
              throw std::logic_error("line isn't artist");
            }
            break;
          case DATA_ARTIST:
            if (line_is("ALBUM", line, pos)) {
              parse(db.ostore(), artist, line, pos);
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
              parse(db.ostore(), album, line, pos);
              data_type = DATA_TRACK;
            } else if (line_is("ALBUMEND", line, pos)) {
              // end artist section
              data_type = DATA_ARTIST;
            } else {
              throw std::logic_error("line isn't track");
            }
            break;
          default:
            throw std::logic_error("unknown data type");
            break;
        }
      }

      tr.commit();
    } catch (std::exception &ex) {
      tr.rollback();
      throw ex;
    }
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

void Import::parse(oos::object_store &ostore, const std::string &line, size_t pos)
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
    // artist already exists
    std::cout << "artist [" << name << "] already exists!\n";
  } else {
    artist_ = ostore.insert(new Artist(name));
  }
}

void Import::parse(oos::object_store &ostore, const artist_ptr &artist, const std::string &line, size_t pos)
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
  } else {
    album_ = ostore.insert(new Album(name, year, artist));
    artist->add(album_);
  }
}

void Import::parse(oos::object_store &ostore, const album_ptr &album, const std::string &line, size_t pos)
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
  } else {
    track_ = ostore.insert(new Track(name, (long)duration, album->artist()));
    album->add(number, track_);
  }
}
