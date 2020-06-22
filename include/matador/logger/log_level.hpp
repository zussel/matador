#ifndef MATADOR_LOG_LEVEL_HPP
#define MATADOR_LOG_LEVEL_HPP

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

}
#endif //MATADOR_LOG_LEVEL_HPP
