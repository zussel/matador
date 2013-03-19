#include "MySQLDatabaseTestUnit.hpp"

using namespace oos;
using namespace std;

#ifdef WIN32
#define MYSQL_DSN "mysql://sascha@192.168.27.57/test"
#else
#define MYSQL_DSN "mysql://sascha:sascha@localhost/test"
#endif

MySQLDatabaseTestUnit::MySQLDatabaseTestUnit()
  : DatabaseTestUnit("mysql database test unit", MYSQL_DSN)
{}

MySQLDatabaseTestUnit::~MySQLDatabaseTestUnit()
{}
