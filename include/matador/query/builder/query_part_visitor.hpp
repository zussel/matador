#ifndef QUERY_QUERY_PART_VISITOR_HPP
#define QUERY_QUERY_PART_VISITOR_HPP

namespace matador::query {
namespace internal {
// Alter common
class query_alter_part;
// Alter table
class query_alter_table_part;
class query_add_key_constraint_part;
class query_drop_key_constraint_part_by_name;
class query_drop_key_constraint_part_by_constraint;
class query_add_foreign_key_constraint_part;
class add_constraint_part_by_constraint;
class query_add_foreign_key_reference_part;
class query_add_primary_key_constraint_part;
// Select
class query_select_part;
class query_select_nextval_part;
class query_select_currval_part;
class query_from_part;
class query_join_table_part;
class query_join_query_part;
class query_on_part;
class query_where_part;
class query_group_by_part;
class query_order_by_part;
class query_order_by_asc_part;
class query_order_by_desc_part;
class query_offset_part;
class query_limit_part;
// Insert
class query_insert_part;
class query_into_part;
class query_values_part;
class query_returning_part;
// Update
class query_update_part;
class query_set_part;
// Delete
class query_delete_part;
class query_delete_from_part;
// Create common
class query_create_part;
// Create table
class query_create_table_part;
class query_create_table_columns_part;
class query_create_table_constraints_part;
// Create Sequence
class query_create_sequence_part;
// Create schema
class query_create_schema_part;
// Drop common
class query_drop_part;
// Drop table
class query_drop_table_part;
// Drop sequence
class query_drop_sequence_part;
// Drop schema
class query_drop_schema_part;
}

class query_part_visitor {
public:
  virtual ~query_part_visitor() = default;

  virtual void visit(internal::query_alter_part &part) = 0;
  virtual void visit(internal::query_alter_table_part &part) = 0;
  virtual void visit(internal::query_add_key_constraint_part &part) = 0;
  virtual void visit(internal::query_add_foreign_key_constraint_part &part) = 0;
  virtual void visit(internal::add_constraint_part_by_constraint &part) = 0;
  virtual void visit(internal::query_add_primary_key_constraint_part &part) = 0;
  virtual void visit(internal::query_add_foreign_key_reference_part &part) = 0;
  virtual void visit(internal::query_drop_key_constraint_part_by_name &part) = 0;
  virtual void visit(internal::query_drop_key_constraint_part_by_constraint &part) = 0;

  virtual void visit(internal::query_select_part &part) = 0;
  virtual void visit(internal::query_select_nextval_part &part) = 0;
  virtual void visit(internal::query_select_currval_part &part) = 0;
  virtual void visit(internal::query_from_part &part) = 0;
  virtual void visit(internal::query_join_table_part &part) = 0;
  virtual void visit(internal::query_join_query_part &part) = 0;
  virtual void visit(internal::query_on_part &part) = 0;
  virtual void visit(internal::query_where_part &part) = 0;
  virtual void visit(internal::query_group_by_part &part) = 0;
  virtual void visit(internal::query_order_by_part &part) = 0;
  virtual void visit(internal::query_order_by_asc_part &part) = 0;
  virtual void visit(internal::query_order_by_desc_part &part) = 0;
  virtual void visit(internal::query_offset_part &part) = 0;
  virtual void visit(internal::query_limit_part &part) = 0;

  virtual void visit(internal::query_insert_part &part) = 0;
  virtual void visit(internal::query_into_part &part) = 0;
  virtual void visit(internal::query_values_part &part) = 0;
  virtual void visit(internal::query_returning_part &part) = 0;

  virtual void visit(internal::query_update_part &part) = 0;
  virtual void visit(internal::query_set_part &part) = 0;

  virtual void visit(internal::query_delete_part &part) = 0;
  virtual void visit(internal::query_delete_from_part &part) = 0;

  virtual void visit(internal::query_create_part &part) = 0;
  virtual void visit(internal::query_create_table_part &part) = 0;
  virtual void visit(internal::query_create_table_columns_part &part) = 0;
  virtual void visit(internal::query_create_table_constraints_part &part) = 0;
  virtual void visit(internal::query_create_sequence_part &part) = 0;
  virtual void visit(internal::query_create_schema_part &part) = 0;

  virtual void visit(internal::query_drop_part &part) = 0;
  virtual void visit(internal::query_drop_table_part &part) = 0;
  virtual void visit(internal::query_drop_sequence_part &part) = 0;
  virtual void visit(internal::query_drop_schema_part &part) = 0;
};

}

#endif //QUERY_QUERY_PART_VISITOR_HPP
