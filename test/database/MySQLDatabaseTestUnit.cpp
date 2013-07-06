#include "MySQLDatabaseTestUnit.hpp"

using namespace oos;
using namespace std;

#ifdef WIN32
#define MYSQL_DSN "mysql://sascha@192.168.27.57/test"
#else
//#define MYSQL_DSN "mysql://sascha:sascha@localhost/test"
#define MYSQL_DSN "mysql://root@l127.0.0.1/oos_test"
#endif

MySQLDatabaseTestUnit::MySQLDatabaseTestUnit()
  : DatabaseTestUnit("mysql database test unit", MYSQL_DSN)
{}

MySQLDatabaseTestUnit::~MySQLDatabaseTestUnit()
{}
