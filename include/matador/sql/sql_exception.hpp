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


  ~sql_exception() override = default;

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  const char* what() const noexcept override
  {
    return what_;
  }

  /**
   * Returns the source of the exception
   *
   * @return The source of this exception.
   */
  const char* source() const
  {
    return source_;
  }

private:
  const char *source_;
  const char *what_;
};

}

#endif //OOS_SQL_EXCEPTION_HPP
