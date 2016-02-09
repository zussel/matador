/**
 * @author sascha on 2/9/16.
 */

#include "sql/sql_exception.hpp"

namespace oos {

sql_exception::sql_exception(const char *source, const char *what)
  : source_(source)
  , what_(what)
{}

sql_exception::~sql_exception()
{

}

const char *sql_exception::what() const noexcept
{
  return what_.c_str();
}

const char *sql_exception::source() const
{
  return source_.c_str();
}

}