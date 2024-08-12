#include <utility>

#include "matador/sql/query_parts.hpp"
#include "matador/sql/basic_condition.hpp"

namespace matador::sql {

query_select_part::query_select_part(std::vector<column> columns)
  : query_part(sql::dialect::token_t::SELECT)
  , columns_(std::move(columns)) {}

void query_select_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

const std::vector<column>& query_select_part::columns() const
{
  return columns_;
}

query_from_part::query_from_part(sql::table t)
  : query_part(sql::dialect::token_t::FROM)
  , table_(std::move(t)) {}

const sql::table &query_from_part::table() const
{
  return table_;
}

void query_from_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_join_part::query_join_part(sql::table t)
  : query_part(sql::dialect::token_t::JOIN)
  , table_(std::move(t)) {}

const sql::table &query_join_part::table() const
{
  return table_;
}

void query_join_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_on_part::query_on_part(std::unique_ptr<basic_condition> &&cond)
: query_part(dialect::token_t::ON)
, condition_(std::move(cond)) {}

const basic_condition &query_on_part::condition() const
{
  return *condition_;
}

void query_on_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_where_part::query_where_part(std::unique_ptr<basic_condition> &&cond)
: query_part(dialect::token_t::WHERE)
, condition_(std::move(cond)) {}

void query_where_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

const basic_condition &query_where_part::condition() const
{
  return *condition_;
}

query_table_name_part::query_table_name_part(sql::dialect::token_t token, std::string table_name)
: query_part(token)
, table_name_(std::move(table_name)) {}

query_group_by_part::query_group_by_part(sql::column col)
: query_part(dialect::token_t::GROUP_BY)
, column_(std::move(col))
{}

const sql::column &query_group_by_part::column() const
{
  return column_;
}

void query_group_by_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_order_by_part::query_order_by_part(sql::column col)
: query_part(dialect::token_t::ORDER_BY)
, column_(std::move(col))
{}

const sql::column &query_order_by_part::column() const
{
  return column_;
}

void query_order_by_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_order_by_asc_part::query_order_by_asc_part()
: query_part(dialect::token_t::ASC)
{}

void query_order_by_asc_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_order_by_desc_part::query_order_by_desc_part()
: query_part(dialect::token_t::DESC)
{}

void query_order_by_desc_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_offset_part::query_offset_part(size_t offset)
: query_part(dialect::token_t::OFFSET)
, offset_(offset) {}

size_t query_offset_part::offset() const
{
  return offset_;
}

void query_offset_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_limit_part::query_limit_part(size_t limit)
: query_part(dialect::token_t::LIMIT)
, limit_(limit) {}

size_t query_limit_part::limit() const
{
  return limit_;
}

void query_limit_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_insert_part::query_insert_part()
: query_part(dialect::token_t::INSERT) {}

void query_insert_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_into_part::query_into_part(sql::table t, std::vector<sql::column> columns)
: query_part(dialect::token_t::INSERT)
, table_(std::move(t))
, columns_(std::move(columns)) {}

const sql::table &query_into_part::table() const
{
  return table_;
}

const std::vector<column> &query_into_part::columns() const
{
  return columns_;
}

void query_into_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_values_part::query_values_part(std::vector<utils::any_type> &&values)
: query_part(sql::dialect::token_t::VALUES)
, values_(std::move(values)) {}

const std::vector<utils::any_type>& query_values_part::values() const
{
  return values_;
}

void query_values_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_update_part::query_update_part(sql::table table)
: query_part(dialect::token_t::UPDATE)
, table_(std::move(table)) {}

const sql::table& query_update_part::table() const
{
  return table_;
}

void query_update_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_set_part::query_set_part(const std::vector<sql::key_value_pair>& key_value_pairs)
: query_part(sql::dialect::token_t::SET)
, key_value_pairs_(key_value_pairs) {}

const std::vector<sql::key_value_pair> &query_set_part::key_values() const
{
  return key_value_pairs_;
}

void query_set_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_delete_part::query_delete_part()
: query_part(sql::dialect::token_t::REMOVE) {}

void query_delete_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_delete_from_part::query_delete_from_part(sql::table table)
: query_part(sql::dialect::token_t::FROM)
, table_(std::move(table)) {}

const sql::table &query_delete_from_part::table() const
{
  return table_;
}

void query_delete_from_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_create_part::query_create_part()
: query_part(sql::dialect::token_t::CREATE) {}

void query_create_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_create_table_part::query_create_table_part(sql::table table, std::vector<sql::column_definition> columns)
: query_part(sql::dialect::token_t::TABLE)
, table_(std::move(table))
, columns_(std::move(columns)) {}

const sql::table &query_create_table_part::table() const
{
  return table_;
}

const std::vector<sql::column_definition> &query_create_table_part::columns() const
{
  return columns_;
}

void query_create_table_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_drop_part::query_drop_part()
: query_part(sql::dialect::token_t::DROP) {}

void query_drop_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

query_drop_table_part::query_drop_table_part(sql::table table)
: query_part(sql::dialect::token_t::TABLE)
, table_(std::move(table)) {}

const sql::table &query_drop_table_part::table() const
{
  return table_;
}

void query_drop_table_part::accept(query_part_visitor &visitor)
{
  visitor.visit(*this);
}

}