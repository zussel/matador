#include "Create.hpp"

#include "database/session.hpp"

#include <stdexcept>

using namespace oos;

Create::Create()
  : Command("create")
{}

Create::~Create()
{}

void Create::execute(session &db, const std::vector<std::string> &/*args*/)
{
  if (!db.is_open()) {
    throw std::logic_error("database isn't open");
  } else {
    db.create();
  }
}
