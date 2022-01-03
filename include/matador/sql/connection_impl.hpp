#ifndef OOS_CONNECTION_IMPL_HPP
#define OOS_CONNECTION_IMPL_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
#define OOS_SQL_API __declspec(dllexport)
#define EXPIMP_SQL_TEMPLATE
#else
#define OOS_SQL_API __declspec(dllimport)
#define EXPIMP_SQL_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/types.hpp"
#include "matador/sql/basic_sql_logger.hpp"
#include "matador/sql/field.hpp"

#include <string>
#include <vector>
#include <memory>

namespace matador {

namespace detail {
class result_impl;
class statement_impl;
}
class sql;
class basic_dialect;

/// @cond MATADOR_DEV

class OOS_SQL_API connection_impl
{
public:
  virtual ~connection_impl() = default;

  virtual void open(const std::string &db) = 0;
  virtual bool is_open() const = 0;
  virtual void close() = 0;

  virtual detail::result_impl* execute(const matador::sql &stmt) = 0;
  virtual detail::result_impl* execute(const std::string &stmt) = 0;
  virtual detail::statement_impl* prepare(const matador::sql &stmt) = 0;

  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual std::string type() const = 0;
  virtual std::string client_version() const = 0;
  virtual std::string server_version() const = 0;

  virtual bool exists(const std::string &table_name) = 0;
  virtual std::vector<field> describe(const std::string &table) = 0;

  virtual basic_dialect* dialect() = 0;

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
