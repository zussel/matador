#include "matador/query/connection.hpp"

#include "matador/query/backend_provider.hpp"
#include "matador/query/dialect.hpp"
#include "matador/query/dialect_token.hpp"
#include "matador/query/interface/connection_impl.hpp"


#include <algorithm>
#include <stdexcept>
#include <utility>

namespace matador::query {

namespace internal {
class connection_statement_proxy final : public statement_proxy {
public:
  explicit connection_statement_proxy(std::unique_ptr<statement_impl>&& stmt)
  : statement_proxy(std::move(stmt)) {}

  utils::result<execute_result, utils::error> execute(value_writer& bindings) override {
    return statement_->execute(bindings);
  }
  utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch(value_writer& bindings) override {
    return statement_->fetch(bindings);
  }
};

}
connection::connection(const connection_info& info, logger_ptr sql_logger)
: logger_(std::move(sql_logger)) {
  connection_.reset(backend_provider::instance().create_connection(info.type, info));
}

connection::connection(const connection_info &info,
                       const std::shared_ptr<resolver_service>& resolver,
                       logger_ptr sql_logger)
: logger_(std::move(sql_logger))
, resolver_service_(resolver) {
  connection_.reset(backend_provider::instance().create_connection(info.type, info));
}

connection::connection(const std::string& dns, const logger_ptr &sql_logger)
: connection(connection_info::parse(dns), sql_logger)
{}

connection::connection(const std::string &dns,
                       const std::shared_ptr<resolver_service> &resolver,
                       const logger_ptr &sql_logger)
: connection(connection_info::parse(dns), resolver, sql_logger)
{}

connection::connection(const connection &x) {
  if (x.connection_) {
    throw std::runtime_error("couldn't copy connection with valid connection impl");
  }
}

connection &connection::operator=(const connection &x) {
  if (this == &x) {
    return *this;
  }

  if (x.connection_) {
    throw std::runtime_error("couldn't copy connection with valid connection impl");
  }
  return *this;
}

connection::connection( connection&& x ) noexcept
: connection_(std::move(x.connection_))
, logger_(std::move(x.logger_))
, resolver_service_(std::move(x.resolver_service_))
{}

connection & connection::operator=(connection &&x) noexcept {
  connection_  = std::move(x.connection_);
  logger_ = std::move(x.logger_);
  resolver_service_ = std::move(x.resolver_service_);

  return *this;
}

connection::~connection() {
  if (!connection_) {
    return;
  }
  if (const auto result = connection_->is_open(); *result) {
    connection_->close();
  }
  connection_impl* impl = connection_.release();
  backend_provider::instance().destroy_connection(impl->info().type, impl);
  connection_ = nullptr;
}

utils::result<void, utils::error> connection::open() const {
  const auto res = is_open();
  if (res.is_error()) {
    return utils::failure(res.err());
  }
  if (!*res) {
    logger_->on_connect();
    return connection_->open();
  }
  return utils::ok<void>();
}

utils::result<void, utils::error> connection::close() const {
  logger_->on_close();
  return connection_->close();
}

utils::result<bool, utils::error> connection::is_open() const
{
  return connection_->is_open();
}

const connection_info &connection::info() const
{
  return connection_->info();
}

std::string connection::type() const {
  return connection_->info().type;
}

utils::result<void, utils::error> connection::begin() const {
  const auto res = connection_->execute({dialect().begin()});
  if (res.is_error()) {
    return utils::failure(res.err());
  }

  return utils::ok<void>();
}

utils::result<void, utils::error> connection::commit() const {
  const auto res = connection_->execute({dialect().commit()});
  if (res.is_error()) {
    return utils::failure(res.err());
  }

  return utils::ok<void>();
}

utils::result<void, utils::error> connection::rollback() const {
  const auto res = connection_->execute({dialect().rollback()});
  if (res.is_error()) {
    return utils::failure(res.err());
  }

  return utils::ok<void>();
}

utils::result<std::vector<column>, utils::error> connection::describe(const std::string &table_name) const {
  return connection_->describe(table_name);
}

utils::result<bool, utils::error> connection::exists(const std::string &schema_name, const std::string &table_name) const {
  return connection_->exists(schema_name, table_name);
}

utils::result<bool, utils::error> connection::exists(const std::string &table_name) const {
  return connection_->exists(dialect().default_schema_name(), table_name);
}

utils::result<bool, utils::error> connection::sequence_exists(const std::string& sequence_name) const {
    return connection_->sequence_exists(dialect().default_schema_name(), sequence_name);
}

static bool has_unknown_columns(const std::vector<column> &columns) {
  return std::any_of(std::begin(columns), std::end(columns), [](const auto &col) {
    return col.type() == utils::basic_type::Null;
  });
}

utils::result<std::unique_ptr<query_result_impl>, utils::error> connection::fetch(const query_context &ctx) const {
  logger_->on_fetch(ctx.sql);
  std::cout << ctx.sql << std::endl;
  return connection_->fetch(ctx);
}

utils::result<execute_result, utils::error> connection::execute(const query_context& ctx) const {
  logger_->on_execute(ctx.sql);
  std::cout << ctx.sql << std::endl;
  return connection_->execute(ctx);
}

utils::result<statement, utils::error> connection::prepare(const query_context &ctx) {
  auto result = perform_prepare(ctx);

  if (!result) {
    return utils::failure(result.err());
  }

  return utils::ok(statement(
    std::make_shared<internal::connection_statement_proxy>(result.release()),
    logger_)
  );
}

std::string connection::str( const query_context& ctx ) const
{
  return ctx.sql;
}

const class dialect &connection::dialect() const
{
  return connection_->dialect();
}

std::shared_ptr<resolver_service> connection::resolver() const {
  return resolver_service_;
}

utils::result<std::unique_ptr<statement_impl>, utils::error> connection::perform_prepare(const query_context& ctx) const {
  if (ctx.command != query_command::CreateTable && (ctx.columns.empty() || has_unknown_columns(ctx.columns))) {
    if (const auto result = describe(ctx.table_name); result.is_ok()) {
      for (auto &col: ctx.columns) {
        const auto rit = std::find_if(
          std::begin(*result),
          std::end(*result),
          [&col, &ctx](const auto &value) {
            if (ctx.table_name.empty()) {
              return value.name() == col.name();
            }
            return value.name() == col.name() || ctx.table_name + "." + value.name() == col.name();
          }
        );
        if (col.type() == utils::basic_type::Null && rit != result->end()) {
          // TODO: Add change_type
          // const_cast<column&>(col).change_type(rit->type());
        }
      }
    }
  }

  auto result = connection_->prepare(ctx);
  if (!result) {
    return utils::failure(result.err());
  }

  return utils::ok(result.release());
}
}
