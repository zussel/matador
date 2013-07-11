#include "SQLiteDatabaseTestUnit.hpp"

#include "connections.hpp"

using namespace oos;
using namespace std;

SQLiteDatabaseTestUnit::SQLiteDatabaseTestUnit()
  : DatabaseTestUnit("sqlite", "sqlite database test unit", connection::sqlite)
{}

SQLiteDatabaseTestUnit::~SQLiteDatabaseTestUnit()
{}
