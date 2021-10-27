#ifndef REACTOR_JSON_EXCEPTION_HPP
#define REACTOR_JSON_EXCEPTION_HPP

#include <string>
#include <stdexcept>

namespace matador {

/**
 * @brief Exception representing json errors
 * 
 * The exception class represents all kind of
 * json errors occuring while parsing a json
 * string.
 */
class json_exception : public std::logic_error
{
public:
  /**
   * Creates a object_exception
   *
   * @param what The message of the exception.
   */
  explicit json_exception(const char *what)
    : std::logic_error(what)
  {}

  /**
   * Destroys the json exception
   */
  ~json_exception() noexcept override = default;
};

}

#endif //REACTOR_JSON_EXCEPTION_HPP
