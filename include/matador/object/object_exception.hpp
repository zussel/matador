#ifndef OBJECT_EXCEPTION_HPP
#define OBJECT_EXCEPTION_HPP

#include <exception>
#include <string>
#include <sstream>

namespace matador {

/**
 * Throws an serializable exception
 * 
 * @param source The source of the exception
 * @param what Detailed information about the exception
 */
//void throw_error(const std::string &source, const std::string &what);

/**
 * @class object_exception
 * @brief An serializable exception class
 *
 * This kind of exception is thrown, when 
 * an serializable dealing error occurs.
 */
class object_exception : public std::runtime_error
{
public:
  /**
   * Creates a object_exception
   *
   * @param what The message of the exception.
   */
  explicit object_exception(const char *what) noexcept
    : std::runtime_error(what)
  {}
};

#define throw_object_exception(message) \
  do { \
    std::stringstream msg; \
    msg << message; \
    throw object_exception(msg.str().c_str()); \
  } while(false)

}

#endif /* OBJECT_EXCEPTION_HPP */
