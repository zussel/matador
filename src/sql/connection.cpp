#include "matador/sql/connection.hpp"

#include "matador/sql/backend_provider.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/schema.hpp"
#include "matador/sql/query_compile_context.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace matador::sql {

connection::connection(connection_info info, const std::shared_ptr<basic_sql_logger> &sqllogger)
: connection_info_(std::move(info))
, logger_(sqllogger)
{
  connection_.reset(backend_provider::instance().create_connection(connection_info_.type, connection_info_));
}

connection::connection(const std::string& dns, const std::shared_ptr<basic_sql_logger> &sqllogger)
: connection(connection_info::parse(dns), sqllogger)
{}

connection::connection(const connection &x)
: connection_info_(x.connection_info_)
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

utils::result<void, sql_error> connection::begin() const {
  const auto res = connection_->execute(dialect().token_at(dialect_token::BEGIN));
  if (res.is_error()) {
    return utils::error(res.err());
  }

  return utils::ok<void>();
}

utils::result<void, sql_error> connection::commit() const {
  const auto res = connection_->execute(dialect().token_at(dialect_token::COMMIT));
  if (res.is_error()) {
    return utils::error(res.err());
  }

  return utils::ok<void>();
}

utils::result<void, sql_error> connection::rollback() const {
  const auto res = connection_->execute(dialect().token_at(dialect_token::ROLLBACK));
  if (res.is_error()) {
    return utils::error(res.err());
  }

  return utils::ok<void>();
}

utils::result<std::vector<column_definition>, sql_error> connection::describe(const std::string &table_name) const
{
  return connection_->describe(table_name);
}

utils::result<bool, sql_error> connection::exists(const std::string &schema_name, const std::string &table_name) const
{
  return connection_->exists(schema_name, table_name);
}

utils::result<bool, sql_error> connection::exists(const std::string &table_name) const
{
  return connection_->exists(dialect().default_schema_name(), table_name);
}

utils::result<size_t, sql_error> connection::execute(const std::string &sql) const
{
//  logger_.debug(sql);
  return connection_->execute(sql);
}

// sql::query connection::query() const
// {
  // return {};
// }

bool is_unknown(const std::vector<sql::column_definition> &columns) {
  return std::all_of(std::begin(columns), std::end(columns), [](const auto &col) {
    return col.type() == data_type::type_unknown;
  });
}

// query_result<record> connection::fetch(const query_context &ctx) const
// {
//   if (ctx.prototype.empty() || is_unknown(ctx.prototype)) {
//     const auto table_prototype = describe(ctx.table.name);
//     for (auto &col : ctx.prototype) {
//       const auto rit = std::find_if(std::begin(table_prototype), std::end(table_prototype), [&col](const auto &value) {
//         return value.name() == col.name();
//       });
//       if (col.type() == data_type::type_unknown && rit != table_prototype.end()) {
//         const_cast<column_definition&>(col).type(rit->type());
//       }
//      }
//   }
// //  auto it = prototypes_.find(q.table_name);
// //  if (it == prototypes_.end()) {
// //    it = prototypes_.emplace(q.table_name, describe(q.table_name)).first;
// //  }
// //  // adjust columns from given query
// //  for (auto &col : q.prototype) {
// //    if (const auto rit = it->second.find(col.name()); col.type() == data_type_t::type_unknown && rit != it->second.end()) {
// //      const_cast<column&>(col).type(rit->type());
// //    }
// //  }
//   auto res = fetch(ctx.sql);
//   return query_result<record>{std::move(res), ctx.prototype};
// }

utils::result<std::unique_ptr<query_result_impl>, sql_error> connection::fetch(const query_compile_context &ctx) const
{
  const auto qry = dialect().compile(ctx, *connection_);
  if (qry.prototype.empty() || is_unknown(qry.prototype)) {
    const auto result = describe(qry.table.name);
    if (!result.is_ok()) {
      return utils::error(result.err());
    }
    for (auto &col : qry.prototype) {
      const auto rit = std::find_if(std::begin(*result), std::end(*result), [&col](const auto &value) {
        return value.name() == col.name();
      });
      if (col.type() == data_type::type_unknown && rit != (*result).end()) {
        const_cast<column_definition&>(col).type(rit->type());
      }
    }
  }

//  logger_.debug(sql);
  return connection_->fetch(qry);
}

utils::result<size_t, sql_error> connection::execute( const query_compile_context& ctx ) const
{
    return execute(dialect().compile(ctx, *connection_).sql);
}

statement connection::prepare(const query_compile_context &query) const
{
    return prepare(dialect().compile(query, *connection_));
}

statement connection::prepare(const query_context &context) const
{
  if (context.command != sql_command::SQL_CMD_CREATE && (context.prototype.empty() || is_unknown(context.prototype))) {
      if (const auto result = describe(context.table.name); result.is_ok()) {
        for (auto &col: context.prototype) {
          const auto rit = std::find_if(std::begin(*result), std::end(*result),
                                        [&col](const auto &value) {
                                          return value.name() == col.name();
                                        });
        if (col.type() == data_type::type_unknown && rit != result->end()) {
          const_cast<column_definition &>(col).type(rit->type());
        }
      }
    }
  }

//  return connection_->prepare(qry).and_then([](auto &&res) {
//    return statement(std::forward<decltype(res)>(res));
//  });
  if (auto result = connection_->prepare(context); result.is_ok()) {
    return statement(result.release());
  }

  return statement(std::unique_ptr<statement_impl>{});
}

std::string connection::str( const query_compile_context& ctx ) const
{
  return dialect().compile(ctx, *connection_).sql;
}

const class dialect &connection::dialect() const
{
  return connection_->dialect();
}

}
