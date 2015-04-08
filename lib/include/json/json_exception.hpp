#ifndef JSON_EXCEPTION_HPP
#define JSON_EXCEPTION_HPP

#include <string>
#include <exception>

namespace oos {

class json_exception : public std::exception
{
  /**
   * Creates a object_exception
   *
   * @param what The message of the exception.
   */
  json_exception(const char *what)
    : what_(what)
  {
  }

  virtual ~json_exception() throw() {}

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  virtual const char* what() const throw()
  {
    return what_.c_str();
  }
private:
  std::string what_;
};

}

#endif /* JSON_EXCEPTION_HPP */