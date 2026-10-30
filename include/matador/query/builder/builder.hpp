#ifndef MATADOR_BUILDER_HPP
#define MATADOR_BUILDER_HPP

#include "matador/query/model/column.hpp"
#include "matador/query/model/constraint.hpp"

#include "matador/utils/types.hpp"

#include <string>

namespace matador::query {
class column_builder {
public:
  explicit column_builder(std::string column_name, utils::basic_type type, size_t size = 0);

  // ReSharper disable once CppNonExplicitConversionOperator
  operator column() const; // NOLINT(*-explicit-constructor)

  column_builder &not_null();
  column_builder &primary_key();
  column_builder &unique();
  column_builder &identity();

private:
  std::string column_name_;
  utils::basic_type type_{};
  column_constraints constraints_{column_constraint::NotNull};
  size_t size_{0};
};

class constraint_builder {
public:
  constraint_builder &constraint(std::string name);
  constraint_builder &primary_key(std::string name);
  constraint_builder &foreign_key(std::string name);
  constraint_builder &references(std::string table, std::string column);

  // ReSharper disable once CppNonExplicitConversionOperator
  operator class constraint() const; // NOLINT(*-explicit-constructor)

private:
  std::string constraint_name_;
  std::string column_name_;
  std::string ref_table_name_;
  std::string ref_column_name_;
  column_constraint type_{};
};

constraint_builder constraint(std::string name);
column_builder col(std::string name, utils::basic_type type, size_t size = 0);
}
#endif //MATADOR_BUILDER_HPP
