#ifndef MYSQL_CONSTANTS_HPP
#define MYSQL_CONSTANTS_HPP

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador {

namespace mysql {

class mysql
{
public:
  static const long version = MYSQL_VERSION_ID;
};

#if MYSQL_VERSION_ID >= 80000
typedef bool mysql_bool;
#else
typedef my_bool bool;
#endif
}
}
#endif /* MYSQL_CONSTANTS_HPP */