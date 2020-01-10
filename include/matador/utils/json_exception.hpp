#ifndef REACTOR_JSON_EXCEPTION_HPP
#define REACTOR_JSON_EXCEPTION_HPP

#include <string>
#include <stdexcept>

namespace matador {
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

  ~json_exception() noexcept override = default;
};
}
#endif //REACTOR_JSON_EXCEPTION_HPP
