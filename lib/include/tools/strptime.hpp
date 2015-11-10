#ifndef STRPTIME_HPP
#define STRPTIME_HPP

#include <time.h>

namespace oos {
namespace detail {
  /*
   * We do not implement alternate representations. However, we always
   * check whether a given modifier is allowed for a certain conversion.
   */
  char* strptime(const char *buf, const char *fmt, struct tm *tm);
}
}
#endif /* STRPTIME_HPP */
