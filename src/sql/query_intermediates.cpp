#include "matador/sql/query_intermediates.hpp"
#include "matador/sql/session.hpp"
#include "matador/sql/query_compiler.hpp"

namespace matador::sql {

query_intermediate::query_intermediate(connection &db, const sql::schema & schema)
: context_(std::make_shared<query_compile_context>(db, schema)) {}

query_intermediate::query_intermediate(const std::shared_ptr<query_compile_context> &context)
: context_(context)
{}

connection &query_intermediate::db() const {
  return context_->db;
}

const class dialect &query_intermediate::dialect() const {
  return context_->db.dialect();
}

const class schema& query_intermediate::schema() const {
  return context_->schema;
}

query_result<record> query_select::fetch_all()
{
//  query_compiler compiler(db().dialect());
  return db().fetch(dialect().compiler().compile(context_.get()));
}

std::optional<record> query_select::fetch_one()
{
//  query_compiler compiler(db().dialect());
  auto result = db().fetch(dialect().compiler().compile(context_.get()));
  auto first = result.begin();
  if (first == result.end()) {
    return std::nullopt;
  }

  return *first.get();
}

query_context query_select::build() const
{
//  query_compiler compiler(db().dialect());
  return dialect().compiler().compile(context_.get());
}

std::unique_ptr<query_result_impl> query_select::fetch()
{
//  query_compiler compiler(db().dialect());
  return db().fetch(dialect().compiler().compile(context_.get()).sql);
}

statement query_select::prepare()
{
//  query_compiler compiler(db().dialect());
  return db().prepare(dialect().compiler().compile(context_.get()));
}

//query_intermediate::query_intermediate(connection &db, const sql::schema &schema, const std::shared_ptr<query_compile_context> &context)
//: basic_query_intermediate(db, schema), context_(context) {}

query_offset_intermediate query_limit_intermediate::offset(size_t offset)
{
  context_->parts.push_back(std::make_unique<query_offset_part>(offset));
  return {context_};
}

query_limit_intermediate query_offset_intermediate::limit(size_t limit)
{
  context_->parts.push_back(std::make_unique<query_limit_part>(limit));
  return {context_};
}

query_limit_intermediate query_order_direction_intermediate::limit(size_t limit)
{
  context_->parts.push_back(std::make_unique<query_limit_part>(limit));
  return {context_};
}

query_order_by_intermediate query_group_by_intermediate::order_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {context_};
}

query_order_direction_intermediate query_order_by_intermediate::asc()
{
  context_->parts.push_back(std::make_unique<query_order_by_asc_part>());
  return {context_};
}

query_order_direction_intermediate query_order_by_intermediate::desc()
{
  context_->parts.push_back(std::make_unique<query_order_by_desc_part>());
  return {context_};
}

query_where_intermediate query_from_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  if (cond) {
    context_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  }
  return {context_};
}

query_group_by_intermediate query_from_intermediate::group_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_group_by_part>(col));
  return {context_};
}

query_order_by_intermediate query_from_intermediate::order_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {context_};
}

query_group_by_intermediate query_where_intermediate::group_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_group_by_part>(col));
  return {context_};
}

query_order_by_intermediate query_where_intermediate::order_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {context_};
}

query_on_intermediate query_join_intermediate::on_clause(std::unique_ptr<basic_condition> &&cond)
{
  context_->parts.push_back(std::make_unique<query_on_part>(std::move(cond)));
  return {context_};
}

query_join_intermediate query_from_intermediate::join_left(const table &t)
{
  context_->parts.push_back(std::make_unique<query_join_part>(t));
  return {context_};
}

query_from_intermediate query_from_intermediate::join_left(join_data &data)
{
  context_->parts.push_back(std::make_unique<query_join_part>(data.join_table));
  context_->parts.push_back(std::make_unique<query_on_part>(std::move(data.condition)));
  return {context_};
}

query_from_intermediate query_from_intermediate::join_left(std::vector<join_data> &data_vector)
{
  for (auto &data : data_vector) {
    context_->parts.push_back(std::make_unique<query_join_part>(data.join_table));
    context_->parts.push_back(std::make_unique<query_on_part>(std::move(data.condition)));
  }
  return {context_};
}

query_select_intermediate::query_select_intermediate(connection &db, const sql::schema &schema, const std::vector<column>& columns)
: query_intermediate(db, schema)
{
  context_->parts.push_back(std::make_unique<query_select_part>(columns));
}

query_from_intermediate query_select_intermediate::from(const table& t)
{
  context_->parts.push_back(std::make_unique<query_from_part>(t));
  return {context_};
}

query_insert_intermediate::query_insert_intermediate(connection &db, const sql::schema &schema)
: query_intermediate(db, schema)
{
  context_->parts.push_back(std::make_unique<query_insert_part>());
}

query_into_intermediate query_insert_intermediate::into(const sql::table &table, std::initializer_list<column> columns)
{
  return into(table, std::move(std::vector<column>{columns}));
}

query_into_intermediate query_insert_intermediate::into(const table &table, std::vector<column> &&columns)
{
  context_->parts.push_back(std::make_unique<query_into_part>(table, columns));
  return {context_};
}

query_into_intermediate query_insert_intermediate::into(const table &table, const std::vector<std::string> &column_names) {
  std::vector<column> columns;
  columns.reserve(column_names.size());
  for (const auto &col_name : column_names) {
    columns.emplace_back(col_name);
  }
  return into(table, std::move(columns));
}

query_into_intermediate query_insert_intermediate::into(const table &table)
{
  context_->parts.push_back(std::make_unique<query_into_part>(table, table.columns));
  return {context_};
}

size_t query_execute::execute()
{
//  query_compiler compiler(db().dialect());
  return db().execute(dialect().compiler().compile(context_.get()).sql);
}

statement query_execute::prepare()
{
//  query_compiler compiler(db().dialect());
  return db().prepare(dialect().compiler().compile(context_.get()));
}

query_context query_execute::build() const
{
//  query_compiler compiler(db().dialect());
  return dialect().compiler().compile(context_.get());
}

query_execute query_into_intermediate::values(std::initializer_list<utils::any_type> values)
{
  return this->values(std::vector<utils::any_type>(values));
}

query_execute query_into_intermediate::values(std::vector<utils::any_type> &&values)
{
  context_->parts.push_back(std::make_unique<query_values_part>(std::move(values)));
  return {context_};
}

query_create_intermediate::query_create_intermediate(connection &db, const sql::schema &schema)
: query_intermediate(db, schema) {
  context_->parts.push_back(std::make_unique<query_create_part>());
}

query_execute query_create_intermediate::table(const sql::table &table, std::initializer_list<column_definition> columns)
{
  return this->table(table, std::vector<column_definition>{columns});
}

query_execute query_create_intermediate::table(const sql::table &table, const std::vector<column_definition> &columns)
{
  context_->parts.push_back(std::make_unique<query_create_table_part>(table, columns));
  return {context_};
}

query_drop_intermediate::query_drop_intermediate(connection &db, const sql::schema &schema)
: query_intermediate(db, schema)
{
  context_->parts.push_back(std::make_unique<query_drop_part>());
}

query_execute query_drop_intermediate::table(const sql::table &table)
{
  context_->parts.push_back(std::make_unique<query_drop_table_part>(table));
  return {context_};
}

query_limit_intermediate query_execute_where_intermediate::limit(size_t limit)
{
  context_->parts.push_back(std::make_unique<query_limit_part>(limit));
  return {context_};
}

query_order_by_intermediate query_execute_where_intermediate::order_by(const column &col)
{
  context_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {context_};
}

query_execute_where_intermediate query_set_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  context_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  return {context_};
}

query_update_intermediate::query_update_intermediate(connection &db, const sql::schema &schema, const sql::table& table)
: query_intermediate(db, schema)
{
  context_->parts.push_back(std::make_unique<query_update_part>(table));
}

query_set_intermediate query_update_intermediate::set(std::initializer_list<key_value_pair> columns)
{
  return set(std::vector<key_value_pair>{columns});
}

query_set_intermediate query_update_intermediate::set(std::vector<key_value_pair> &&columns)
{
  context_->parts.push_back(std::make_unique<query_set_part>(std::move(columns)));
  return {context_};

}

query_execute_where_intermediate query_delete_from_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  context_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  return {context_};
}

query_delete_intermediate::query_delete_intermediate(connection &db, const sql::schema &schema)
: query_intermediate(db, schema)
{
  context_->parts.push_back(std::make_unique<query_delete_part>());
}

query_delete_from_intermediate query_delete_intermediate::from(const sql::table &table)
{
  context_->parts.push_back(std::make_unique<query_delete_from_part>(table));
  return {context_};
}

//query_intermediate::query_intermediate(connection &db, const sql::schema &schema)
//: basic_query_intermediate(db, schema)
//, context_(std::make_shared<query_compile_context>(db().dialect()))
//{}
}