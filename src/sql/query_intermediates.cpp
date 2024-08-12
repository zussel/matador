#include "matador/sql/query_intermediates.hpp"
#include "matador/sql/session.hpp"
#include "matador/sql/query_compiler.hpp"
#include "matador/sql/condition.hpp"

namespace matador::sql {
basic_query_intermediate::basic_query_intermediate(connection &db, const sql::schema & schema)
: connection_(db)
, schema_(schema) {}

query_result<record> query_select::fetch_all()
{
  query_compiler compiler(connection_.dialect());
  return connection_.fetch(compiler.compile(data_.get()));
}

std::optional<record> query_select::fetch_one()
{
  query_compiler compiler(connection_.dialect());
  auto result = connection_.fetch(compiler.compile(data_.get()));
  auto first = result.begin();
  if (first == result.end()) {
    return std::nullopt;
  }

  return *first.get();
}

query_context query_select::build() const
{
  query_compiler compiler(connection_.dialect());
  return compiler.compile(data_.get());
}

std::unique_ptr<query_result_impl> query_select::fetch()
{
  query_compiler compiler(connection_.dialect());
  return connection_.fetch(compiler.compile(data_.get()).sql);
}

statement query_select::prepare()
{
  query_compiler compiler(connection_.dialect());
  return connection_.prepare(compiler.compile(data_.get()));
}

query_intermediate::query_intermediate(connection &db, const sql::schema &schema, const std::shared_ptr<query_data> &data)
: basic_query_intermediate(db, schema), data_(data) {}

query_offset_intermediate query_limit_intermediate::offset(size_t offset)
{
  data_->parts.push_back(std::make_unique<query_offset_part>(offset));
  return {connection_, schema_, data_};
}

query_limit_intermediate query_offset_intermediate::limit(size_t limit)
{
  data_->parts.push_back(std::make_unique<query_limit_part>(limit));
  return {connection_, schema_, data_};
}

query_limit_intermediate query_order_direction_intermediate::limit(size_t limit)
{
  data_->parts.push_back(std::make_unique<query_limit_part>(limit));
  return {connection_, schema_, data_};
}

query_order_by_intermediate query_group_by_intermediate::order_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {connection_, schema_, data_};
}

query_order_direction_intermediate query_order_by_intermediate::asc()
{
  data_->parts.push_back(std::make_unique<query_order_by_asc_part>());
  return {connection_, schema_, data_};
}

query_order_direction_intermediate query_order_by_intermediate::desc()
{
  data_->parts.push_back(std::make_unique<query_order_by_desc_part>());
  return {connection_, schema_, data_};
}

query_where_intermediate query_from_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  if (cond) {
    data_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  }
  return {connection_, schema_, data_};
}

query_group_by_intermediate query_from_intermediate::group_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_group_by_part>(col));
  return {connection_, schema_, data_};
}

query_order_by_intermediate query_from_intermediate::order_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {connection_, schema_, data_};
}

query_group_by_intermediate query_where_intermediate::group_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_group_by_part>(col));
  return {connection_, schema_, data_};
}

query_order_by_intermediate query_where_intermediate::order_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {connection_, schema_, data_};
}

query_on_intermediate query_join_intermediate::on_clause(std::unique_ptr<basic_condition> &&cond)
{
  data_->parts.push_back(std::make_unique<query_on_part>(std::move(cond)));
  return {connection_, schema_, data_};
}

query_join_intermediate query_from_intermediate::join_left(const table &t)
{
  data_->parts.push_back(std::make_unique<query_join_part>(t));
  return {connection_, schema_, data_};
}

query_from_intermediate query_from_intermediate::join_left(join_data &data)
{
  data_->parts.push_back(std::make_unique<query_join_part>(data.join_table));
  data_->parts.push_back(std::make_unique<query_on_part>(std::move(data.condition)));
  return {connection_, schema_, data_};
}

query_from_intermediate query_from_intermediate::join_left(std::vector<join_data> &data_vector)
{
  for (auto &data : data_vector) {
    data_->parts.push_back(std::make_unique<query_join_part>(data.join_table));
    data_->parts.push_back(std::make_unique<query_on_part>(std::move(data.condition)));
  }
  return {connection_, schema_, data_};
}

query_select_intermediate::query_select_intermediate(connection &db, const sql::schema &schema, const std::vector<column>& columns)
: query_start_intermediate(db, schema)
{
  data_->parts.push_back(std::make_unique<query_select_part>(columns));
}

query_from_intermediate query_select_intermediate::from(const table& t)
{
  data_->parts.push_back(std::make_unique<query_from_part>(t));
  return {connection_, schema_, data_};
}

query_insert_intermediate::query_insert_intermediate(connection &db, const sql::schema &schema)
: query_start_intermediate(db, schema)
{
  data_->parts.push_back(std::make_unique<query_insert_part>());
}

query_into_intermediate query_insert_intermediate::into(const sql::table &table, std::initializer_list<column> column_names)
{
  return into(table, std::move(std::vector<column>{column_names}));
}

query_into_intermediate query_insert_intermediate::into(const table &table, std::vector<column> &&column_names)
{
  data_->parts.push_back(std::make_unique<query_into_part>(table, column_names));
  return {connection_, schema_, data_};
}

query_into_intermediate query_insert_intermediate::into(const table &table)
{
  data_->parts.push_back(std::make_unique<query_into_part>(table, table.columns));
  return {connection_, schema_, data_};
}

size_t query_execute::execute()
{
  query_compiler compiler(connection_.dialect());
  return connection_.execute(compiler.compile(data_.get()).sql);
}

statement query_execute::prepare()
{
  query_compiler compiler(connection_.dialect());
  return connection_.prepare(compiler.compile(data_.get()));
}

query_context query_execute::build() const
{
  query_compiler compiler(connection_.dialect());
  return compiler.compile(data_.get());
}

query_execute query_into_intermediate::values(std::initializer_list<utils::any_type> values)
{
  return this->values(std::vector<utils::any_type>(values));
}

query_execute query_into_intermediate::values(std::vector<utils::any_type> &&values)
{
  data_->parts.push_back(std::make_unique<query_values_part>(std::move(values)));
  return {connection_, schema_, data_};
}

query_create_intermediate::query_create_intermediate(connection &db, const sql::schema &schema)
: query_start_intermediate(db, schema) {
  data_->parts.push_back(std::make_unique<query_create_part>());
}

query_execute query_create_intermediate::table(const sql::table &table, std::initializer_list<column_definition> columns)
{
  return this->table(table, std::vector<column_definition>{columns});
}

query_execute query_create_intermediate::table(const sql::table &table, const std::vector<column_definition> &columns)
{
  data_->parts.push_back(std::make_unique<query_create_table_part>(table, columns));
  return {connection_, schema_, data_};
}

query_drop_intermediate::query_drop_intermediate(connection &db, const sql::schema &schema)
: query_start_intermediate(db, schema)
{
  data_->parts.push_back(std::make_unique<query_drop_part>());
}

query_execute query_drop_intermediate::table(const sql::table &table)
{
  data_->parts.push_back(std::make_unique<query_drop_table_part>(table));
  return {connection_, schema_, data_};
}

query_order_by_intermediate query_execute_where_intermediate::order_by(const column &col)
{
  data_->parts.push_back(std::make_unique<query_order_by_part>(col));
  return {connection_, schema_, data_};
}

query_execute_where_intermediate query_set_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  data_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  return {connection_, schema_, data_};
}

query_update_intermediate::query_update_intermediate(connection &db, const sql::schema &schema, const sql::table& table)
: query_start_intermediate(db, schema)
{
  data_->parts.push_back(std::make_unique<query_update_part>(table));
}

query_set_intermediate query_update_intermediate::set(std::initializer_list<key_value_pair> columns)
{
  return set(std::vector<key_value_pair>{columns});
}

query_set_intermediate query_update_intermediate::set(std::vector<key_value_pair> &&columns)
{
  data_->parts.push_back(std::make_unique<query_set_part>(std::move(columns)));
  return {connection_, schema_, data_};

}

query_execute_where_intermediate query_delete_from_intermediate::where_clause(std::unique_ptr<basic_condition> &&cond)
{
  data_->parts.push_back(std::make_unique<query_where_part>(std::move(cond)));
  return {connection_, schema_, data_};
}

query_delete_intermediate::query_delete_intermediate(connection &db, const sql::schema &schema)
: query_start_intermediate(db, schema)
{
  data_->parts.push_back(std::make_unique<query_delete_part>());
}

query_delete_from_intermediate query_delete_intermediate::from(const sql::table &table)
{
  data_->parts.push_back(std::make_unique<query_delete_from_part>(table));
  return {connection_, schema_, data_};
}

query_start_intermediate::query_start_intermediate(connection &db, const sql::schema &schema)
: basic_query_intermediate(db, schema)
{}
}