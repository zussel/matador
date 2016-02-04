#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "sql/result.hpp"
#include "sql/statement.hpp"
#include "sql/connection_impl.hpp"

#include <string>

namespace oos {

class connection
{
public:
  explicit connection(const std::string &dns);
  ~connection();

  /**
   * @brief Opens the database.
   *
   * Opens the database. If database couldn't be opened
   * an exception is thrown.
   */
  void open();

  /**
   * @brief Returns true if database is open.
   *
   * Returns true if database is open
   *
   * @return True on open database.
   */
  bool is_open() const;

  /**
   * @brief Closes the database.
   *
   * Closes the database.
   */
  void close();

  template < class T >
  result<T> execute(const std::string &stmt)
  {
    return result<T>(impl_->execute(stmt));
  }

  template < class T >
  statement<T> prepare(const oos::sql &sql)
  {
    return statement<T>(impl_->prepare(sql));
  }

private:
  std::string type_;
  std::string dns_;
  connection_impl *impl_;
};

}
#endif /* CONNECTION_HPP */
