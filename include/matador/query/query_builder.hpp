#ifndef QUERY_QUERY_COMPILER_HPP
#define QUERY_QUERY_COMPILER_HPP

#include "matador/query/query_part_visitor.hpp"
#include "matador/query/query_data.hpp"

#include "matador/query/query_context.hpp"
#include "matador/query/interface/connection_impl.hpp"

#include "matador/query/placeholder.hpp"

#include <functional>
#include <optional>

namespace matador::query {
class dialect;
class constraint;
struct query_data;
struct value_visitor;

namespace internal {
struct basic_type_to_string_visitor;
}

class query_builder final : public query_part_visitor {
public:
  query_context build(const query_data &data,
                      const dialect &d,
                      std::optional<std::reference_wrapper<const connection_impl>> conn);

  void visit(internal::query_alter_part& part) override;
  void visit(internal::query_alter_table_part& part) override;
  void visit(internal::query_add_key_constraint_part& part) override;
  void visit(internal::query_add_foreign_key_constraint_part& part) override;
  void visit(internal::query_add_primary_key_constraint_part& part) override;
  void visit(internal::query_add_foreign_key_reference_part& part) override;
  void visit(internal::query_add_constraint_part_by_constraint &part) override;
  void visit(internal::query_drop_key_constraint_part_by_name &part) override;
  void visit(internal::query_drop_key_constraint_part_by_constraint &part) override;

  void visit(internal::query_select_part &part) override;
  void visit(internal::query_select_nextval_part &part) override;
  void visit(internal::query_select_currval_part &part) override;
  void visit(internal::query_from_part &part) override;
  void visit(internal::query_join_table_part &part) override;
  void visit(internal::query_join_query_part &part) override;
  void visit(internal::query_on_part &part) override;
  void visit(internal::query_where_part &part) override;
  void visit(internal::query_group_by_part &part) override;
  void visit(internal::query_order_by_part &part) override;
  void visit(internal::query_order_by_asc_part &part) override;
  void visit(internal::query_order_by_desc_part &part) override;
  void visit(internal::query_offset_part &part) override;
  void visit(internal::query_limit_part &part) override;

  void visit(internal::query_insert_part &part) override;
  void visit(internal::query_into_part &part) override;
  void visit(internal::query_values_part &part) override;
  void visit(internal::query_returning_part &part) override;

  void visit(internal::query_update_part &part) override;
  void visit(internal::query_set_part &part) override;

  void visit(internal::query_delete_part &part) override;
  void visit(internal::query_delete_from_part &part) override;

  void visit(internal::query_create_part &part) override;
  void visit(internal::query_create_table_part &part) override;
  void visit(internal::query_create_table_columns_part& part) override;
  void visit(internal::query_create_table_constraints_part& part) override;
  void visit(internal::query_create_sequence_part &part) override;
  void visit(internal::query_create_schema_part& part) override;

  void visit(internal::query_drop_part &part) override;
  void visit(internal::query_drop_table_part &part) override;
  void visit(internal::query_drop_sequence_part &part) override;
  void visit(internal::query_drop_schema_part& part) override;

  static std::string build_table_name(dialect_token token, const dialect &d, const std::string& table_name);
  static std::string build_table_name(dialect_token token, const dialect &d, const table& t);
  static std::string build_table_name(const dialect &d, const table& t);
  static std::string determine_value(const dialect &d, query_context& ctx, const abstract_column_expression &exp);
  static std::string determine_value(value_visitor &visitor, const std::variant<placeholder, utils::database_type> &val);


  [[nodiscard]] std::string build_add_constraint_string(const constraint& c) const;
  [[nodiscard]] std::string build_drop_constraint_string(const constraint& c) const;
  static std::string build_constraint_name(const constraint& c);

protected:
  const query_data *data_{};
  query_context query_;
  size_t table_index{0};
  const dialect *dialect_{nullptr};
  std::optional<std::reference_wrapper<const connection_impl>> connection_{};

  std::function<void(query_context&)> finisher_ = [](query_context&) {};
};

}

#endif //QUERY_QUERY_COMPILER_HPP
