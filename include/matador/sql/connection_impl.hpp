#ifndef QUERY_CONNECTION_IMPL_HPP
#define QUERY_CONNECTION_IMPL_HPP

#include "matador/sql/connection_info.hpp"
#include "matador/sql/query_result_impl.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/record.hpp"
#include "matador/sql/sql_error.hpp"
#include "matador/sql/statement_impl.hpp"

#include "matador/utils/version.hpp"

#include <memory>

namespace matador::sql {

class query_result_impl;

class connection_impl
{
public:
  virtual ~connection_impl() = default;

  virtual void open() = 0;
  virtual void close() = 0;
  [[nodiscard]] virtual bool is_open() const = 0;
  [[nodiscard]] virtual bool is_valid() const = 0;

  [[nodiscard]] virtual version client_version() const = 0;
  [[nodiscard]] virtual version server_version() const = 0;

  virtual utils::result<size_t, sql_error> execute(const std::string &stmt) = 0;
  virtual std::unique_ptr<query_result_impl> fetch(const query_context& context) = 0;
  virtual std::unique_ptr<statement_impl> prepare(query_context context) = 0;

  virtual std::vector<column_definition> describe(const std::string &table) = 0;
  virtual bool exists(const std::string &schema_name, const std::string &table_name) = 0;

  [[nodiscard]] const class dialect &dialect() const;

  [[nodiscard]] virtual std::string to_escaped_string(const utils::blob &value) const = 0;

protected:
  explicit connection_impl(const connection_info &info);

  [[nodiscard]] const connection_info &info() const;

private:
  std::reference_wrapper<const connection_info> info_;
  std::reference_wrapper<const class dialect> dialect_;
};

}
#endif //QUERY_CONNECTION_IMPL_HPP
