#include "matador/sql/connection.hpp"

#include "matador/sql/backend_provider.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/schema.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace matador::sql {

connection::connection(connection_info info, const std::shared_ptr<basic_sql_logger> &sqllogger)
: connection_info_(std::move(info))
, dialect_(backend_provider::instance().connection_dialect(connection_info_.type))
, logger_(sqllogger)
{
  connection_.reset(backend_provider::instance().create_connection(connection_info_.type, connection_info_));
}

connection::connection(const std::string& dns, const std::shared_ptr<basic_sql_logger> &sqllogger)
: connection(connection_info::parse(dns), sqllogger)
{}

connection::connection(const connection &x)
: connection_info_(x.connection_info_)
, dialect_(x.dialect_)
{
  if (x.connection_) {
    throw std::runtime_error("couldn't copy connection with valid connection impl");
  }
}

connection &connection::operator=(const connection &x) {
  if (this == &x) {
    return *this;
  }

  connection_info_ = x.connection_info_;
  if (x.connection_) {
    throw std::runtime_error("couldn't copy connection with valid connection impl");
  }
  return *this;
}

connection & connection::operator=(connection &&x) noexcept {
  connection_info_ = std::move(x.connection_info_);
  connection_  = std::move(x.connection_);
  dialect_ = x.dialect_;
  logger_ = std::move(x.logger_);

  return *this;
}

connection::~connection()
{
  if (connection_->is_open()) {
    connection_->close();
  }
  backend_provider::instance().destroy_connection(connection_info_.type, connection_.release());
  connection_ = nullptr;
}

void connection::open() const
{
  if (is_open()) {
    return;
  }
  logger_->on_connect();
  connection_->open();
}

void connection::close() const
{
  logger_->on_close();
  connection_->close();
}

bool connection::is_open() const
{
  return connection_->is_open();
}

const connection_info &connection::info() const
{
  return connection_info_;
}

std::string connection::type() const {
  return connection_info_.type;
}

void connection::begin() const {
  connection_->execute(dialect_.get().token_at(dialect_token::BEGIN));
}

void connection::commit() const {
  connection_->execute(dialect_.get().token_at(dialect_token::COMMIT));
}

void connection::rollback() const {
  connection_->execute(dialect_.get().token_at(dialect_token::ROLLBACK));
}

std::vector<sql::column_definition> connection::describe(const std::string &table_name) const
{
  return connection_->describe(table_name);
}

bool connection::exists(const std::string &schema_name, const std::string &table_name) const
{
  return connection_->exists(schema_name, table_name);
}

bool connection::exists(const std::string &table_name) const
{
  return connection_->exists(dialect_.get().default_schema_name(), table_name);
}

size_t connection::execute(const std::string &sql) const
{
//  logger_.debug(sql);
  return connection_->execute(sql);
}

sql::query connection::query(const sql::schema &schema) const
{
  return sql::query(*const_cast<connection*>(this), schema);
}

bool is_unknown(const std::vector<sql::column_definition> &columns) {
  return std::all_of(std::begin(columns), std::end(columns), [](const auto &col) {
    return col.type() == data_type::type_unknown;
  });
}

query_result<record> connection::fetch(const query_context &q) const
{
  if (q.prototype.empty() || is_unknown(q.prototype)) {
    const auto table_prototype = describe(q.table.name);
    for (auto &col : q.prototype) {
      const auto rit = std::find_if(std::begin(table_prototype), std::end(table_prototype), [&col](const auto &value) {
        return value.name() == col.name();
      });
      if (col.type() == data_type::type_unknown && rit != table_prototype.end()) {
        const_cast<column_definition&>(col).type(rit->type());
      }
     }
  }
//  auto it = prototypes_.find(q.table_name);
//  if (it == prototypes_.end()) {
//    it = prototypes_.emplace(q.table_name, describe(q.table_name)).first;
//  }
//  // adjust columns from given query
//  for (auto &col : q.prototype) {
//    if (const auto rit = it->second.find(col.name()); col.type() == data_type_t::type_unknown && rit != it->second.end()) {
//      const_cast<column&>(col).type(rit->type());
//    }
//  }
  auto res = fetch(q.sql);
  return query_result<record>{std::move(res), q.prototype};
}

std::unique_ptr<query_result_impl> connection::fetch(const std::string &sql) const
{
//  logger_.debug(sql);
  return connection_->fetch(sql);
}

statement connection::prepare(query_context &&query) const
{
  return statement(connection_->prepare(std::move(query)));
}

const class dialect &connection::dialect() const
{
  return dialect_;
}

}
