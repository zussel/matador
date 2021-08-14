#ifndef MATADOR_LOG_LEVEL_HPP
#define MATADOR_LOG_LEVEL_HPP

#ifdef _MSC_VER
#ifdef matador_logger_EXPORTS
#define OOS_LOGGER_API __declspec(dllexport)
#define EXPIMP_LOGGER_TEMPLATE
#else
#define OOS_LOGGER_API __declspec(dllimport)
#define EXPIMP_LOGGER_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_LOGGER_API
#endif

#include <iosfwd>

namespace matador {

/**
 * Represents all available log levels
 */
enum class log_level
{
  LVL_FATAL,  /**< If a serious error occurred use FATAL level */
  LVL_ERROR,  /**< On error use ERROR level */
  LVL_WARN,   /**< Warnings should use WARN level */
  LVL_INFO,   /**< Information should go with INFO level */
  LVL_DEBUG,  /**< Debug output should use DEBUG level */
  LVL_TRACE,  /**< Trace information should use TRACE level */
  LVL_ALL     /**< This level represents all log levels and should be used for logging */
};

/**
 * Write log level in human readable string
 * to a given std::ostream.
 *
 * @param os std::stream to write to
 * @param lvl Log level to write
 * @return The std::ostream
 */
OOS_LOGGER_API std::ostream& operator<<(std::ostream &os, log_level lvl);

/// @cond MATADOR_DEV

struct log_level_range
{
  log_level min_level = log_level::LVL_INFO;
  log_level max_level = log_level::LVL_FATAL;
};

/// @endcond

}
#endif //MATADOR_LOG_LEVEL_HPP
