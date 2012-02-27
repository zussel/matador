#include "Artist.hpp"
#include "Album.hpp"
#include "Track.hpp"

#include "object/object_store.hpp"
#include "object/object_view.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"

#include <string>
#include <fstream>

using namespace oos;

bool read_music_data(database &db, const char *fname);
bool line_is(const char *token, const std::string &line, size_t &pos);
void parse_artist(object_store &ostore, const std::string &line, size_t pos);
void parse_album(object_store &ostore, const std::string &line, size_t pos);
void parse_track(object_store &ostore, const std::string &line, size_t pos);

int
main(int argc, char *argv[])
{
  // create object store
  object_store ostore;

  // setup object hierarchy
  ostore.insert_prototype<Artist>("artist");
  ostore.insert_prototype<Album>("album");
  ostore.insert_prototype<Track>("track");

  // create and open db
  database db(ostore, "sqlite://mdb.sqlite");

  // load db
  db.load();

  // read in data from file
  read_music_data(db, "musicdata.txt");

  // close db
}

bool
read_music_data(database &db, const char *fname)
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

  transaction tr(db);
  try {
    tr.begin();

    std::string line;
    while (std::getline(fin, line)) {
      size_t pos = 0;
      switch (data_type) {
        case DATA_NONE:
          if (line_is("ARTIST", line, pos)) {
            parse_artist(db.ostore(), line, pos);
            data_type = DATA_ARTIST;
          } else {
            throw std::logic_error("line isn't artist");
          }
          break;
        case DATA_ARTIST:
          if (line_is("ALBUM", line, pos)) {
            parse_album(db.ostore(), line, pos);
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
            parse_track(db.ostore(), line, pos);
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

void parse_artist(object_store &ostore, const std::string &line, size_t pos)
{
  pos = line.find('[', pos);
  if (pos == std::string::npos) {
    throw std::logic_error("parse artist: couldn't find opening data bracket");
  }
  pos = line.find('"', pos);
  if (pos == std::string::npos) {
    throw std::logic_error("parse artist: couldn't find starting name quote");
  }
  size_t end = line.find('"', ++pos);
  if (end == std::string::npos) {
    throw std::logic_error("parse artist: couldn't find ending name quote");
  }

  std::string name = line.substr(pos, end-pos);

  object_view<Artist> oview(ostore);
  if (oview.find_if(&Artist::name, name) != oview.end()) {
    // artist already exists
    std::cout << "artist [" << name << "] already exists!\n";
  } else {
    ostore.insert(new Artist(name));
  }
}

void parse_album(object_store &ostore, const std::string &line, size_t pos)
{
}

void parse_track(object_store &ostore, const std::string &line, size_t pos)
{
}
