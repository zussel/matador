#include "matador/query/internal/query_parts.hpp"

#include "matador/query/query_part_visitor.hpp"

#include <utility>

namespace matador::query::internal {
query_alter_part::query_alter_part()
: query_part(dialect_token::Alter) {
}

void query_alter_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_alter_table_part::query_alter_table_part(class table tab)
: query_part(dialect_token::Table)
, table_(std::move(tab)) {
}

void query_alter_table_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const class table &query_alter_table_part::table() const {
  return table_;
}

query_add_key_constraint_part::query_add_key_constraint_part(std::string name)
: query_part(dialect_token::AddConstraint)
, name_(std::move(name)) {
}

void query_add_key_constraint_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const std::string &query_add_key_constraint_part::name() const {
  return name_;
}

add_constraint_part_by_constraint::add_constraint_part_by_constraint(const class constraint &co)
: query_part(dialect_token::AddConstraint)
, constraint_(co) {
}

void add_constraint_part_by_constraint::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const class constraint &add_constraint_part_by_constraint::constraint() const {
  return constraint_;
}

query_drop_key_constraint_part_by_name::query_drop_key_constraint_part_by_name(std::string name)
: query_part(dialect_token::DropConstraint)
, name_(std::move(name)) {
}

void query_drop_key_constraint_part_by_name::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const std::string &query_drop_key_constraint_part_by_name::name() const {
  return name_;
}

query_drop_key_constraint_part_by_constraint::query_drop_key_constraint_part_by_constraint(const class constraint &co)
: query_part(dialect_token::DropConstraint)
, constraint_(co) {
}

void query_drop_key_constraint_part_by_constraint::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const constraint &query_drop_key_constraint_part_by_constraint::constraint() const {
  return constraint_;
}

query_add_foreign_key_constraint_part::query_add_foreign_key_constraint_part(const std::vector< column> &columns)
: query_part(dialect_token::ForeignKey)
, columns_(columns) {
}

void query_add_foreign_key_constraint_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const std::vector< column> &query_add_foreign_key_constraint_part::columns() const {
  return columns_;
}

query_add_foreign_key_reference_part::query_add_foreign_key_reference_part(class table tab, const std::vector< column> &columns)
: query_part(dialect_token::References)
, table_(std::move(tab))
, columns_(columns) {
}

void query_add_foreign_key_reference_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const table &query_add_foreign_key_reference_part::table() const {
  return table_;
}

const std::vector< column> &query_add_foreign_key_reference_part::columns() const {
  return columns_;
}

query_add_primary_key_constraint_part::query_add_primary_key_constraint_part(const std::vector< column> &columns)
: query_part(dialect_token::PrimaryKey)
, columns_(columns) {
}

void query_add_primary_key_constraint_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const std::vector< column> &query_add_primary_key_constraint_part::columns() const {
  return columns_;
}

query_select_part::query_select_part(std::vector< column> columns)
: query_part(dialect_token::Select)
, columns_(std::move(columns)) {
}

void query_select_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const std::vector< column> &query_select_part::columns() const {
  return columns_;
}

query_select_nextval_part::query_select_nextval_part(std::string sequence_name)
: query_part(dialect_token::NextVal)
, sequence_name_(std::move(sequence_name)){
}

const std::string& query_select_nextval_part::sequence_name() const {
  return sequence_name_;
}

void query_select_nextval_part::accept(query_part_visitor& visitor) {
  visitor.visit(*this);
}

query_select_currval_part::query_select_currval_part(std::string sequence_name)
: query_part(dialect_token::CurrVal)
, sequence_name_(std::move(sequence_name)){
}

const std::string& query_select_currval_part::sequence_name() const {
  return sequence_name_;
}

void query_select_currval_part::accept(query_part_visitor& visitor) {
  visitor.visit(*this);
}

query_from_part::query_from_part(std::vector<table> tables)
: query_part(dialect_token::From)
, tables_(std::move(tables)) {
}

const std::vector<table> &query_from_part::tables() const {
  return tables_;
}

void query_from_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_join_table_part::query_join_table_part(class table tab)
: query_part(dialect_token::Join)
, table_(std::move(tab)) {
}

const class table &query_join_table_part::table() const {
  return table_;
}

void query_join_table_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_join_query_part::query_join_query_part(query_context  ctx)
: query_part(dialect_token::Join)
, ctx_(std::move(ctx))
{}

const query_context &query_join_query_part::query() const {
  return ctx_;
}

void query_join_query_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_on_part::query_on_part(std::unique_ptr<abstract_criteria> &&cond)
: query_part(dialect_token::On)
, condition_(std::move(cond)) {
}

const abstract_criteria &query_on_part::condition() const {
  return *condition_;
}

void query_on_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_where_part::query_where_part(std::unique_ptr<abstract_criteria> &&cond)
: query_part(dialect_token::Where)
, condition_(std::move(cond)) {
}

void query_where_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

const abstract_criteria &query_where_part::condition() const {
  return *condition_;
}

table_name_part::table_name_part(const dialect_token token, std::string table_name)
: query_part(token)
, table_name_(std::move(table_name)) {
}

query_group_by_part::query_group_by_part(const std::vector< column> &columns)
: query_part(dialect_token::GroupBy)
, columns_(columns) {
}

const std::vector< column> &query_group_by_part::columns() const {
  return columns_;
}

void query_group_by_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_order_by_part::query_order_by_part(const std::vector< column>& columns)
: query_part(dialect_token::OrderBy)
, columns_(columns) {
}

const std::vector< column> &query_order_by_part::columns() const {
  return columns_;
}

void query_order_by_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_order_by_asc_part::query_order_by_asc_part()
: query_part(dialect_token::Asc) {
}

void query_order_by_asc_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_order_by_desc_part::query_order_by_desc_part()
: query_part(dialect_token::Desc) {
}

void query_order_by_desc_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_offset_part::query_offset_part(size_t offset)
: query_part(dialect_token::Offset)
, offset_(offset) {
}

size_t query_offset_part::offset() const {
  return offset_;
}

void query_offset_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_limit_part::query_limit_part(size_t limit)
: query_part(dialect_token::Limit)
, limit_(limit) {
}

size_t query_limit_part::limit() const {
  return limit_;
}

void query_limit_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_insert_part::query_insert_part()
: query_part(dialect_token::Insert) {
}

void query_insert_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_into_part::query_into_part(class table tab, std::vector< column> columns)
: query_part(dialect_token::Insert)
, table_(std::move(tab))
, columns_(std::move(columns)) {
}

const class table &query_into_part::table() const {
  return table_;
}

const std::vector< column> &query_into_part::columns() const {
  return columns_;
}

void query_into_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_values_part::query_values_part(std::vector<std::variant<placeholder, utils::database_type> > &&values)
: query_part(dialect_token::Values)
, values_(std::move(values)) {
}

const std::vector<std::variant<placeholder, utils::database_type> > &query_values_part::values() const {
  return values_;
}

void query_values_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_returning_part::query_returning_part(std::vector< column> columns)
: query_part(dialect_token::Returning)
, columns_(std::move(columns)) {
}

const std::vector< column> & query_returning_part::columns() const {
  return columns_;
}

void query_returning_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_update_part::query_update_part(class table tab)
: query_part(dialect_token::Update)
, table_(std::move(tab)) {
}

const class table &query_update_part::table() const {
  return table_;
}

void query_update_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_set_part::query_set_part(std::vector<column_value_pair> &&key_value_pairs)
: query_part(dialect_token::Set)
, key_value_pairs_(std::move(key_value_pairs)) {
}

const std::vector<column_value_pair> &query_set_part::column_values() const {
  return key_value_pairs_;
}

void query_set_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_delete_part::query_delete_part()
: query_part(dialect_token::Remove) {
}

void query_delete_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_delete_from_part::query_delete_from_part(class table tab)
: query_part(dialect_token::From)
, table_(std::move(tab)) {
}

const class table &query_delete_from_part::table() const {
  return table_;
}

void query_delete_from_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_create_part::query_create_part()
: query_part(dialect_token::Create) {
}

void query_create_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_create_table_part::query_create_table_part(class table tab)
: query_part(dialect_token::Table)
, table_(std::move(tab)) {
}

const class table &query_create_table_part::table() const {
  return table_;
}

void query_create_table_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_create_table_columns_part::query_create_table_columns_part(const std::list< column> &columns)
: query_part(dialect_token::Columns)
, columns_(columns) {
}

const std::list< column> &query_create_table_columns_part::columns() const {
  return columns_;
}

void query_create_table_columns_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_create_table_constraints_part::query_create_table_constraints_part(
  const std::list<class constraint> &constraints)
: query_part(dialect_token::Constraint)
, constraints_(constraints) {
}

const std::list<class constraint> &query_create_table_constraints_part::constraints() const {
  return constraints_;
}

void query_create_table_constraints_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_create_sequence_part::query_create_sequence_part(std::string sequence_name)
: query_part(dialect_token::Sequence)
, sequence_name_(std::move(sequence_name)) {}

const std::string& query_create_sequence_part::sequence_name() const {
  return sequence_name_;
}

void query_create_sequence_part::accept(query_part_visitor& visitor) {
  visitor.visit(*this);
}

query_create_schema_part::query_create_schema_part(std::string schema)
: query_part(dialect_token::Schema)
, schema_(std::move(schema)) {
}

const std::string &query_create_schema_part::schema() const {
  return schema_;
}

void query_create_schema_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_drop_part::query_drop_part()
: query_part(dialect_token::Drop) {
}

void query_drop_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_drop_table_part::query_drop_table_part(class table tab)
: query_part(dialect_token::Table)
, table_(std::move(tab)) {
}

const class table &query_drop_table_part::table() const {
  return table_;
}

void query_drop_table_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}

query_drop_sequence_part::query_drop_sequence_part(std::string sequence_name)
: query_part(dialect_token::Table)
, sequence_name_(std::move(sequence_name)) {
}

const std::string& query_drop_sequence_part::sequence_name() const {
  return sequence_name_;
}

void query_drop_sequence_part::accept(query_part_visitor& visitor) {
  visitor.visit(*this);
}

query_drop_schema_part::query_drop_schema_part(std::string schema_)
: query_part(dialect_token::Schema)
, schema_(std::move(schema_)) {
}

const std::string &query_drop_schema_part::schema() const {
  return schema_;
}

void query_drop_schema_part::accept(query_part_visitor &visitor) {
  visitor.visit(*this);
}
}
