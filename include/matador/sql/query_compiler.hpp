#ifndef QUERY_QUERY_COMPILER_HPP
#define QUERY_QUERY_COMPILER_HPP

#include "matador/sql/query_part_visitor.hpp"
#include "matador/sql/query_parts.hpp"
#include "matador/sql/query_context.hpp"

namespace matador::sql {

class dialect;

struct query_compile_context;

class query_compiler : public query_part_visitor
{
public:
  query_context compile(const query_compile_context *data);

protected:
  void visit(query_select_part &select_part) override;
  void visit(query_from_part &from_part) override;
  void visit(query_join_part &join_part) override;
  void visit(query_on_part &on_part) override;
  void visit(query_where_part &where_part) override;
  void visit(query_group_by_part &group_by_part) override;
  void visit(query_order_by_part &order_by_part) override;
  void visit(query_order_by_asc_part &order_by_asc_part) override;
  void visit(query_order_by_desc_part &order_by_desc_part) override;
  void visit(query_offset_part &offset_part) override;
  void visit(query_limit_part &limit_part) override;
  void visit(query_insert_part &insert_part) override;
  void visit(query_into_part &into_part) override;
  void visit(query_values_part &values_part) override;

  void visit(query_update_part &update_part) override;
  void visit(query_set_part &set_part) override;

  void visit(query_delete_part &delete_part) override;
  void visit(query_delete_from_part &delete_from_part) override;

  void visit(query_create_part &create_part) override;
  void visit(query_create_table_part &create_table_part) override;

  void visit(query_drop_part &drop_part) override;
  void visit(query_drop_table_part &drop_table_part) override;

  static std::string build_table_name(dialect_token token, const dialect &d, const table& t);

protected:
  const query_compile_context *data_{};
  query_context query_;
};

}

#endif //QUERY_QUERY_COMPILER_HPP
