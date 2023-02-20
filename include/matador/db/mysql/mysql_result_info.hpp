#ifndef MYSQL_RESULT_INFO_HPP
#define MYSQL_RESULT_INFO_HPP

#include "matador/db/mysql/mysql_bool.hpp"

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
  my_bool is_null = false;
  my_bool error = false;
  std::unique_ptr<char[]> buffer;
//  char *buffer = nullptr;
  unsigned long buffer_length = 0;
//  bool is_allocated = false;

//  ~mysql_result_info()
//  {
//      if (is_allocated) {
//          delete [] buffer;
//      }
//  }
};

}

}

#endif // MYSQL_RESULT_INFO_HPP

