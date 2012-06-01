#include "Add.hpp"

#include "database/session.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"

#include <iostream>

using namespace oos;

Add::Add()
  : Command("add")
{}

Add::~Add()
{}

void Add::execute(oos::session &db, const std::vector<std::string> &args)
{
  if (!db.is_open()) {
    throw std::logic_error("database isn't open");
  } else {
    if (args.empty()) {
      throw std::logic_error("missing list argument");
    }

    //db.load();

    // add artist name="Yellow"
    // add track album=1 title="Yadada" duration=300
    // add album artist=5 name="Blue" year=1999

    // find prototype by name
    // check fields
    // set fields

    object *o = db.ostore().create(args[0].c_str());

    if (o == 0) {
      throw std::logic_error("unknown type [" + args[0] + "]");
    }

    o = db.ostore().insert(o).get();

    // set values

    typedef std::vector<std::string> string_vector_t;
    string_vector_t::const_iterator first = args.begin();
    string_vector_t::const_iterator last = args.end();

    while (first != last) {
      std::cout << "arg: " << *first++ << "\n";
    }
  }
}
