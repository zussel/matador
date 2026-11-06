#include "matador/utils/date.hpp"

#include <chrono>

namespace matatdor {
date::date()
: date(now()) {}

date::date(const current_date_parts parts)
: year_(year_type{parts.year})
, month_(month_type{parts.month})
, day_(day_type{parts.day}) {
}

date::current_date_parts date::now() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t time = std::chrono::system_clock::to_time_t(now);

  std::tm local_time{};

#if defined(_WIN32)
  localtime_s(&local_time, &time);
#else
  localtime_r(&time, &local_time);
#endif

  return {
    local_time.tm_year + 1900,
    static_cast<unsigned>(local_time.tm_mon + 1),
    static_cast<unsigned>(local_time.tm_mday)
};
}
}