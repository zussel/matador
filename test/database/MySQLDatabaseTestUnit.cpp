#include "MySQLDatabaseTestUnit.hpp"

using namespace oos;
using namespace std;

MySQLDatabaseTestUnit::MySQLDatabaseTestUnit()
  : DatabaseTestUnit("mysql database test unit", "mysql://sascha:sascha@localhost/test")
{}

MySQLDatabaseTestUnit::~MySQLDatabaseTestUnit()
{}
