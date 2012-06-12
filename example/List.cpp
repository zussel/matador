#include "Artist.hpp"
#include "Album.hpp"
#include "Track.hpp"

#include "List.hpp"
#include "TablePrinter.hpp"

#include "database/session.hpp"

#include <iostream>

using namespace oos;
using namespace std;

List::List()
  : Command("list")
{}

List::~List()
{}

void List::execute(oos::session &db, const std::vector<std::string> &args)
{
  if (!db.is_open()) {
    throw std::logic_error("database isn't open");
  } else {
    if (args.empty()) {
      throw std::logic_error("missing list argument");
    }

    db.load();
    
    /*
     * usage:
     * mdb list track artist=1
     * mdb list track duration>200
     * mdb list artist name="Genesis"
     * mdb list album name="xxxx"
     */
    TablePrinter printer(db.ostore());
    
    if (args.size() == 2) {
      printer.print(args[0], args[1]);
    } else {
      printer.print(args[0]);
    }

    /*
    generic_view gview(db.ostore(), "artist");
    
    generic_view::iterator first = gview.begin();
    generic_view::iterator last = gview.end();
    */

    if (args[0] == "artist") {
      printer.print<Artist>(cout);
    } else if (args[0] == "album") {
      printer.print<Album>(cout);
    } else if (args[0] == "track") {
      printer.print<Track>(cout);
    } else {
      throw std::logic_error("invalid list type");
    }
  }
}
