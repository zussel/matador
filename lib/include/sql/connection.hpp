#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "sql/result.hpp"
#include "sql/statement.hpp"
#include "sql/connection_impl.hpp"
#include "row.hpp"

#include <string>

namespace oos {

class basic_dialect;

class connection
{
public:
  explicit connection(const std::string &dns);
  ~connection();

  /**
   * @brief Opens the sql.
   *
   * Opens the sql. If sql couldn't be opened
   * an exception is thrown.
   */
  void open();

  /**
   * @brief Returns true if sql is open.
   *
   * Returns true if sql is open
   *
   * @return True on open sql.
   */
  bool is_open() const;

  /**
   * @brief Closes the sql.
   *
   * Closes the sql.
   */
  void close();

  void begin();
  void commit();
  void rollback();

  void execute(const std::string &stmt)
  {
    std::unique_ptr<detail::result_impl> res(impl_->execute(stmt));
  }

  template < class T >
  result<T> execute(const sql &stmt)
  {
    return result<T>(impl_->execute(stmt));
  }

  result<row> execute(const sql &stmt, const row &prototype)
  {
    return result<row>(impl_->execute(stmt));
  }

  template < class T >
  statement<T> prepare(const oos::sql &sql)
  {
    return statement<T>(impl_->prepare(sql));
  }

  const char* type_string(data_type_t type) const;

  bool exists(const std::string &tablename) const;

  basic_dialect* dialect();

private:
  std::string type_;
  std::string dns_;
  connection_impl *impl_;
};

}
#endif /* CONNECTION_HPP */
