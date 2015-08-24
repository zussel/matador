#ifndef MYSQL_RESULT_INFO_HPP
#define MYSQL_RESULT_INFO_HPP

#ifdef WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace oos {

namespace mysql {

struct mysql_result_info
{
  unsigned long length;
  my_bool is_null;
  my_bool error;
  char *buffer;
  unsigned long buffer_length;
};

}

}

#endif // MYSQL_RESULT_INFO_HPP

