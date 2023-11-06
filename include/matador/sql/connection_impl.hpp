#ifndef OOS_CONNECTION_IMPL_HPP
#define OOS_CONNECTION_IMPL_HPP

#include "matador/sql/types.hpp"
#include "matador/sql/basic_sql_logger.hpp"
#include "matador/sql/field.hpp"

#include "matador/utils/version.hpp"

#include <string>
#include <vector>
#include <memory>

namespace matador {

namespace detail {
class result_impl;
class statement_impl;
struct statement_context;
}
class sql;
class basic_dialect;
struct connection_info;

/// @cond MATADOR_DEV

class connection_impl
{
public:
  virtual ~connection_impl() = default;

  virtual void open(const connection_info &info) = 0;
  virtual bool is_open() const = 0;
  virtual void close() = 0;

  virtual detail::result_impl* execute(const matador::sql &stmt) = 0;
  virtual detail::result_impl* execute(const std::string &stmt) = 0;
  virtual detail::statement_impl* prepare(detail::statement_context &&context) const = 0;

  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual std::string type() const = 0;
  virtual version client_version() const = 0;
  virtual version server_version() const = 0;

  virtual bool exists(const std::string &table_name) = 0;
  virtual std::vector<field> describe(const std::string &table) = 0;

  virtual basic_dialect* dialect() = 0;

  virtual unsigned short default_port() const = 0;

  /**
   * Enable console log of sql statements
   */
  void enable_log();

  /**
   * Disable console log of sql statements
   */
  void disable_log();

  /**
   * Returns true if logging is enabled.
   *
   * @return True if logging is enabled
   */
  bool is_log_enabled() const;

private:
  bool log_enabled_ = false;
};

/// @endcond

}
#endif //OOS_CONNECTION_IMPL_HPP
