#ifndef QUERY_QUERY_PART_VISITOR_HPP
#define QUERY_QUERY_PART_VISITOR_HPP

namespace matador::sql {

class query_select_part;
class query_from_part;
class query_join_part;
class query_on_part;
class query_where_part;
class query_group_by_part;
class query_order_by_part;
class query_order_by_asc_part;
class query_order_by_desc_part;
class query_offset_part;
class query_limit_part;
class query_insert_part;
class query_into_part;
class query_values_part;
class query_update_part;
class query_set_part;
class query_delete_part;
class query_delete_from_part;
class query_create_part;
class query_create_table_part;
class query_drop_part;
class query_drop_table_part;

class query_part_visitor
{
public:
  virtual ~query_part_visitor() = default;

  virtual void visit(query_select_part &select_part) = 0;
  virtual void visit(query_from_part &from_part) = 0;
  virtual void visit(query_join_part &join_part) = 0;
  virtual void visit(query_on_part &on_part) = 0;
  virtual void visit(query_where_part &where_part) = 0;
  virtual void visit(query_group_by_part &group_by_part) = 0;
  virtual void visit(query_order_by_part &order_by_part) = 0;
  virtual void visit(query_order_by_asc_part &order_by_asc_part) = 0;
  virtual void visit(query_order_by_desc_part &order_by_desc_part) = 0;
  virtual void visit(query_offset_part &offset_part) = 0;
  virtual void visit(query_limit_part &limit_part) = 0;

  virtual void visit(query_insert_part &insert_part) = 0;
  virtual void visit(query_into_part &into_part) = 0;
  virtual void visit(query_values_part &values_part) = 0;

  virtual void visit(query_update_part &update_part) = 0;
  virtual void visit(query_set_part &set_part) = 0;

  virtual void visit(query_delete_part &delete_part) = 0;
  virtual void visit(query_delete_from_part &delete_from_part) = 0;

  virtual void visit(query_create_part &create_part) = 0;
  virtual void visit(query_create_table_part &create_table_part) = 0;

  virtual void visit(query_drop_part &drop_part) = 0;
  virtual void visit(query_drop_table_part &drop_table_part) = 0;
};

}

#endif //QUERY_QUERY_PART_VISITOR_HPP
