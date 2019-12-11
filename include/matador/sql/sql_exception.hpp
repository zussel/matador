//
// Created by sascha on 2/9/16.
//

#ifndef OOS_SQL_EXCEPTION_HPP
#define OOS_SQL_EXCEPTION_HPP

#include <exception>
#include <string>

namespace matador {

/**
 * @brief An serializable exception class
 *
 * This kind of exception is thrown, when
 * an serializable dealing error occurs.
 */
class sql_exception : public std::exception
{
public:
  /**
   * Creates a database_exception
   *
   * @param source The source of the exception.
   * @param what The message of the exception.
   */
  sql_exception(const char *source, const char *what)
    : source_(source)
    , what_(what)
  {}


  /**
   * Creates a database_exception
   *
   * @param source The source of the exception.
   * @param what The message of the exception.
   */
  sql_exception(std::string source, std::string what)
    : source_(std::move(source))
    , what_(std::move(what))
  {}


  ~sql_exception() override = default;

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  const char* what() const noexcept override
  {
    return what_.c_str();
  }

  /**
   * Returns the source of the exception
   *
   * @return The source of this exception.
   */
  const char* source() const
  {
    return source_.c_str();
  }

private:
  std::string source_;
  std::string what_;
};

}

#endif //OOS_SQL_EXCEPTION_HPP
