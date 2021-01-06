#include "matador/logger/log_level.hpp"

#include <iostream>

namespace matador {

std::ostream& operator<<(std::ostream &os, log_level lvl)
{
  switch (lvl) {
    case log_level::LVL_ERROR:
      os << "ERROR";
      break;
    case log_level::LVL_FATAL:
      os << "FATAL";
      break;
    case log_level::LVL_DEBUG:
      os << "DEBUG";
      break;
    case log_level::LVL_INFO:
      os << "INFO";
      break;
    case log_level::LVL_TRACE:
      os << "TRACE";
      break;
    case log_level::LVL_WARN:
      os << "WARN";
      break;
    case log_level::LVL_ALL:
      os << "ALL";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

}
