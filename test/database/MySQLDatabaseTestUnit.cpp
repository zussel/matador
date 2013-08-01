#include "MySQLDatabaseTestUnit.hpp"

#include "connections.hpp"

using namespace oos;
using namespace std;

MySQLDatabaseTestUnit::MySQLDatabaseTestUnit()
  : DatabaseTestUnit("mysql", "mysql database test unit", connection::mysql)
{}

MySQLDatabaseTestUnit::~MySQLDatabaseTestUnit()
{}
