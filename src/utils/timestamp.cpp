#include "matador/utils/timestamp.hpp"

#include <chrono>
#include <ctime>

namespace matatdor {
timestamp::timestamp()
: timestamp(now())
{
}

timestamp::current_timestamp_parts timestamp::now() {
  const auto now = std::chrono::system_clock::now();

  const auto duration = now.time_since_epoch();
  const auto milliseconds_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  const auto millisecond =
      static_cast<unsigned>(milliseconds_since_epoch % 1000);

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
    static_cast<unsigned>(local_time.tm_mday),
    static_cast<unsigned>(local_time.tm_hour),
    static_cast<unsigned>(local_time.tm_min),
    static_cast<unsigned>(local_time.tm_sec),
    millisecond
};
}
}