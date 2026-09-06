#ifndef QUERY_CONNECTION_IMPL_HPP
#define QUERY_CONNECTION_IMPL_HPP

#include "matador/query/column.hpp"

#include "matador/query/dialect.hpp"
#include "matador/query/connection_info.hpp"
#include "matador/query/execute_result.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"
#include "matador/utils/version.hpp"

#include <memory>

namespace matador::utils {
using blob_type_t = std::vector<unsigned char>;
}

namespace matador::query {
struct query_context;
class query_result_impl;
class statement_impl;

class connection_impl {
public:
  virtual ~connection_impl() = default;

  virtual result<void, error> open() = 0;
  virtual result<void, error> close() = 0;
  [[nodiscard]] virtual result<bool, error> is_open() const = 0;
  [[nodiscard]] virtual result<bool, error> is_valid() const = 0;

  [[nodiscard]] virtual result<utils::version, error> client_version() const = 0;
  [[nodiscard]] virtual result<utils::version, error> server_version() const = 0;

  virtual result<execute_result, error> execute(const query_context &context) = 0;
  virtual result<std::unique_ptr<query_result_impl>, error> fetch(const query_context &context) = 0;
  virtual result<std::unique_ptr<statement_impl>, error> prepare(const query_context &context) = 0;

  virtual result<std::vector<column>, error> describe(const std::string &table) = 0;
  virtual result<bool, error> exists(const std::string &schema_name, const std::string &table_name) = 0;

  virtual result<bool, error> sequence_exists(const std::string &schema_name, const std::string &sequence_name) = 0;

  [[nodiscard]] const class dialect &dialect() const;

  [[nodiscard]] virtual std::string to_escaped_string(const utils::blob_type_t &value) const = 0;
  [[nodiscard]] const connection_info &info() const;

protected:
  explicit connection_impl(const connection_info &info);

private:
  connection_info info_;
  std::reference_wrapper<const class dialect> dialect_;
};

}
#endif //QUERY_CONNECTION_IMPL_HPP
