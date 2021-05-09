#ifndef MYSQL_CONSTANTS_HPP
#define MYSQL_CONSTANTS_HPP

#ifdef _MSC_VER
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

}
}
#endif /* MYSQL_CONSTANTS_HPP */