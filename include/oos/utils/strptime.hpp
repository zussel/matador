#ifndef STRPTIME_HPP
#define STRPTIME_HPP

#include <time.h>

namespace oos {
namespace detail {
  /**
   * @brief Convert a string representation of time to a time tm structure
   *
   * @note We do not implement alternate representations. However, we always
   * check whether a given modifier is allowed for a certain conversion.
   *
   * @param buf The string buffer with the time representation
   * @param fmt The time format string
   * @param tm The tm struct to fill
   * @return Pointer to the first character not processed
   */
  char* strptime(const char *buf, const char *fmt, struct tm *tm);
}
}
#endif /* STRPTIME_HPP */
