#ifndef OBJECT_EXCEPTION_HPP
#define OBJECT_EXCEPTION_HPP

#include <exception>
#include <string>

class object;

class object_exception : public std::exception
{
public:
  object_exception(const char *what, object *o)
    : what_(what)
    , obj_(o)
  {
  }
  virtual ~object_exception() throw() {}
  virtual const char* what() const throw()
  {
    return what_.c_str();
  }
private:
  std::string what_;
  object *obj_;
};

#endif /* OBJECT_EXCEPTION_HPP */
