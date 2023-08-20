#ifndef SQLITE_EXCEPTION_HPP
#define SQLITE_EXCEPTION_HPP

#ifdef _MSC_VER
  #ifdef matador_sqlite_EXPORTS
    #define OOS_SQLITE_API __declspec(dllexport)
  #else
    #define OOS_SQLITE_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_SQLITE_API
#endif

#include <sqlite3.h>

namespace matador {

namespace sqlite {

void throw_database_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql = "");

}

}

#endif /* SQLITE_EXCEPTION_HPP */
