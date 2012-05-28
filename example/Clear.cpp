#include "Clear.hpp"

#include "database/session.hpp"

#include <stdexcept>

using namespace oos;

Clear::Clear()
  : Command("clear")
{}

Clear::~Clear()
{}

void Clear::execute(session &db, const std::vector<std::string> &/*args*/)
{
  if (!db.is_open()) {
    throw std::logic_error("database isn't open");
  } else {
    db.drop();
  }
}
