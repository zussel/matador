#include "MSSQLDatabaseTestUnit.hpp"

#include "connections.hpp"

using namespace oos;
using namespace std;

MSSQLDatabaseTestUnit::MSSQLDatabaseTestUnit()
  : DatabaseTestUnit("mssql", "mssql database test unit", connection::mssql)
{}

MSSQLDatabaseTestUnit::~MSSQLDatabaseTestUnit()
{}
