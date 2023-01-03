#ifndef MATADOR_LOG_SINK_HPP
#define MATADOR_LOG_SINK_HPP

#include "matador/logger/export.hpp"

#include <cstddef>
#include <memory>

namespace matador {

/**
 * @brief Base class for all log sinks
 *
 * This class must be the base class for all
 * log sinks and provides their interface
 *
 * The main interface is the write(...) interface
 * defining how the log message is written.
 *
 * The close() interface defines a way to close
 * the concrete log sink
 */
class OOS_LOGGER_API log_sink
{
public:
  /**
   * Destroys the log sink
   */
  virtual ~log_sink() = default;

  /**
   * Writes the given log message with the given size
   * to the concrete sink
   *
   * @param message The message to log
   * @param size The size of the message
   */
  virtual void write(const char *message, std::size_t size) = 0;

  /**
   * Closes the log sink if necessary.
   */
  virtual void close() = 0;
};

using sink_ptr = std::shared_ptr<log_sink>; /**< Shortcut to log sink shared pointer */

}

#endif //MATADOR_LOG_SINK_HPP
