#ifndef REACTOR_JSON_EXCEPTION_HPP
#define REACTOR_JSON_EXCEPTION_HPP

#include <exception>
#include <string>

namespace matador {
class json_exception : public std::exception
{
  /**
   * Creates a object_exception
   *
   * @param what The message of the exception.
   */
  explicit json_exception(const char *what)
    : what_(what)
  {
  }

  ~json_exception() noexcept override = default;

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  const char* what() const noexcept override
  {
    return what_.c_str();
  }
private:
  std::string what_;
};
}
#endif //REACTOR_JSON_EXCEPTION_HPP
