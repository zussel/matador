#ifndef MYSQL_RESULT_INFO_HPP
#define MYSQL_RESULT_INFO_HPP

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <iostream>

namespace matador {

namespace mysql {

struct mysql_result_info
{
  unsigned long length = 0;
  bool is_null = false;
  bool error = false;
  char *buffer = nullptr;
  unsigned long buffer_length = 0;
  bool is_allocated = false;

  ~mysql_result_info()
  {
      if (is_allocated) {
          delete [] buffer;
      }
  }
};

}

}

#endif // MYSQL_RESULT_INFO_HPP

