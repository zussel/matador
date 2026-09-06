#ifndef QUERY_QUERY_DATA_HPP
#define QUERY_QUERY_DATA_HPP

#include "matador/query/column.hpp"

// #include "matador/sql/resolver_service.hpp"

#include <typeindex>

namespace matador::query {
enum class query_command : uint8_t{
  Unknown,
  Create,
  CreateTable,
  CreateSchema,
  CreateSequence,
  CreateDatabase,
  Update,
  Insert,
  Delete,
  Select,
  Drop,
  DropTable,
  DropSchema,
  DropSequence,
  DropDatabase,
  Alter,
  AlterTable,
  AlterSchema
};

struct query_context {
  std::string sql;
  size_t sql_hash{};
  query_command command{};
  std::string schema_name;
  std::string table_name;
  std::vector<column> columns{};
  std::vector<std::string> bind_vars;
  // Data for resolving query result
  // std::shared_ptr<resolver_service> resolver{};
  std::type_index result_type = typeid(void);

  [[nodiscard]] bool is_ddl() const { return command == query_command::Create || command == query_command::Alter; }
  [[nodiscard]] bool is_dml() const { return command == query_command::Insert || command == query_command::Update || command == query_command::Delete; }
  [[nodiscard]] bool is_query() const { return command == query_command::Select; }
};
}

#endif //QUERY_QUERY_DATA_HPP
