#include "Artist.hpp"
#include "Album.hpp"
#include "Track.hpp"

#include "Create.hpp"
#include "Clear.hpp"
#include "Import.hpp"
#include "List.hpp"

#include "database/session.hpp"

#include "tools/factory.hpp"

#include <iostream>

using namespace oos;
using namespace std;

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    // print usage
    exit(0);
  }

  // setup command factory
  typedef factory<std::string, Command> command_factory_t;

  command_factory_t cfactory;

  /*********************
   *
   * setup command server
   * with following commands:
   * - create
   * - clear
   * - import
   * - export
   * - list [artist,album,track]
   * - add [artist,album,track]
   * - delete [artist,album,track]
   * - update [artist,album,track]
   *
   *********************/
  cfactory.insert("create", new command_factory_t::default_producer<Create>);
  cfactory.insert("clear", new command_factory_t::default_producer<Clear>);
  cfactory.insert("import", new command_factory_t::default_producer<Import>);
  cfactory.insert("list", new command_factory_t::default_producer<List>);

  // create object store
  object_store ostore;

  // setup object hierarchy
  ostore.insert_prototype<Artist>("artist");
  ostore.insert_prototype<Track>("track");
  ostore.insert_prototype<Album>("album");
  ostore.insert_prototype<Album::item_type>("track_album_rel");
  ostore.insert_prototype<Artist::item_type>("album_artist_rel");

  // create and open db
  session db(ostore, "sqlite://mdb.sqlite");


  Command *cmd = cfactory.create(argv[1]);

  if (!cmd) {
    cout << "unknown command [" << argv[1] << "]\n";
    exit(1);
  }

  // concatenate all parameters to one string
  std::vector<std::string> args(argv+2, argv+argc);

  try {
    cmd->execute(db, args);
  } catch (exception &ex) {
    cout << "command error: " << ex.what() << "\n";
  }

  delete cmd;

  // read in data from file
//  read_music_data(db, "musicdata.txt");

  // close db
  return 0;
}
