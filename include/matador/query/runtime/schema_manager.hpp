#ifndef MATADOR_SCHEMA_MANAGER_HPP
#define MATADOR_SCHEMA_MANAGER_HPP

#include "matador/utils/result.hpp"
#include "matador/utils/error.hpp"

namespace matador::query {
class connection;
class basic_schema;

class schema_manager final {
public:
  explicit schema_manager(basic_schema& scm);

  [[nodiscard]] result<void, error> create(const connection &conn) const;
  [[nodiscard]] result<void, error> drop(const connection &conn) const;

  [[nodiscard]] result<bool, error> table_exists(const std::string &table_name, const connection &conn) const;

private:
  basic_schema& schema_;
};
}
#endif // MATADOR_SCHEMA_MANAGER_HPP
