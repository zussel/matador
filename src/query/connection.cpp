#include "matador/query/connection.hpp"

#include "matador/query/backend_provider.hpp"
#include "matador/query/dialect.hpp"
#include "matador/query/interface/connection_impl.hpp"

#include <algorithm>
#include <utility>

namespace matador::query {

namespace {
class connection_statement_proxy final : public statement_proxy {
public:
  explicit connection_statement_proxy(std::unique_ptr<statement_impl> &&stmt)
      : statement_proxy(std::move(stmt)) {}

  utils::result<execute_result, utils::error> execute(value_writer &bindings) override {
    return statement_->execute(bindings);
  }
  utils::result<std::unique_ptr<query_result_impl>, utils::error>
  fetch(value_writer &bindings) override {
    return statement_->fetch(bindings);
  }
};

connection::logger_ptr normalize_logger(const connection::logger_ptr& logger) {
  return logger ? logger : null_logger;
}

std::shared_ptr<resolver_service> normalize_resolver(std::shared_ptr<resolver_service> resolver) {
  return resolver ? std::move(resolver) : std::make_shared<resolver_service>();
}

utils::result<void, utils::error> to_void_result(const utils::result<execute_result, utils::error> &result) {
  if (result.is_error()) {
    return utils::failure(result.err());
  }

  return utils::ok<void>();
}
} // namespace

void connection::connection_deleter::operator()(connection_impl *impl) const noexcept {
  if (!impl) {
    return;
  }

  try {
    const auto type = impl->info().type;
    backend_provider::instance().destroy_connection(type, impl);
  } catch (...) {
    // Deleters used by unique_ptr destructors must not throw.
  }
}

connection::connection(const connection_info &info, const logger_ptr& sql_logger)
: logger_(normalize_logger(sql_logger)) {
  connection_.reset(backend_provider::instance().create_connection(info.type, info));
}

connection::connection(const connection_info &info,
                       std::shared_ptr<resolver_service> resolver,
                       const logger_ptr& sql_logger)
: logger_(normalize_logger(sql_logger))
, resolver_service_(normalize_resolver(std::move(resolver))) {
  connection_.reset(backend_provider::instance().create_connection(info.type, info));
}

connection::connection(const std::string &dns, const logger_ptr &sql_logger)
: connection(connection_info::parse(dns), sql_logger) {}

connection::connection(const std::string &dns, const std::shared_ptr<resolver_service> &resolver,
                       const logger_ptr &sql_logger)
: connection(connection_info::parse(dns), resolver, sql_logger) {}

connection::~connection() {
  if (!connection_) {
    return;
  }

  try {
    const auto open_result = connection_->is_open();
    if (open_result.is_ok() && *open_result) {
      const auto close_result = connection_->close();
      if (!close_result && logger_) {
        logger_->on_error(close_result.err());
      }
    }
  } catch (...) {
    // Destructors must not emit exceptions.
  }
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
  const auto result = is_open();
  if (result.is_error()) {
    return utils::failure(result.err());
  }

  if (*result) {
    logger_->on_close();
    return connection_->close();
  }

  return utils::ok<void>();
}

utils::result<bool, utils::error> connection::is_open() const {
  return connection_->is_open();
}

const connection_info &connection::info() const {
  return connection_->info();
}

std::string connection::type() const {
  return connection_->info().type;
}

utils::result<void, utils::error> connection::begin() const {
  return to_void_result(execute({dialect().begin()}));
}

utils::result<void, utils::error> connection::commit() const {
  return to_void_result(execute({dialect().commit()}));
}

utils::result<void, utils::error> connection::rollback() const {
  return to_void_result(execute({dialect().rollback()}));
}

utils::result<std::vector<column>, utils::error>
connection::describe(const std::string &table_name) const {
  return connection_->describe(table_name);
}

utils::result<bool, utils::error> connection::exists(const std::string &schema_name,
                                                     const std::string &table_name) const {
  return connection_->exists(schema_name, table_name);
}

utils::result<bool, utils::error> connection::exists(const std::string &table_name) const {
  return connection_->exists(dialect().default_schema_name(), table_name);
}

utils::result<bool, utils::error>
connection::sequence_exists(const std::string &sequence_name) const {
  return connection_->sequence_exists(dialect().default_schema_name(), sequence_name);
}

static bool has_unknown_columns(const std::vector<column> &columns) {
  return std::any_of(std::begin(columns), std::end(columns),
                     [](const auto &col) { return col.type() == utils::basic_type::Null; });
}

utils::result<std::unique_ptr<query_result_impl>, utils::error> connection::fetch(const query_context &ctx) const {
  logger_->on_fetch(ctx.sql);
  return connection_->fetch(ctx);
}

utils::result<execute_result, utils::error> connection::execute(const query_context &ctx) const {
  logger_->on_execute(ctx.sql);
  return connection_->execute(ctx);
}

utils::result<statement, utils::error> connection::prepare(const query_context &ctx) const {
  logger_->on_prepare(ctx.sql);

  auto result = perform_prepare(ctx);
  if (!result) {
    return utils::failure(result.err());
  }

  return ok<statement>(statement(std::make_shared<connection_statement_proxy>(result.release()), logger_));
}

std::string connection::str(const query_context &ctx) const {
  return ctx.sql;
}

const class dialect &connection::dialect() const {
  return connection_->dialect();
}

std::shared_ptr<resolver_service> connection::resolver() const {
  return resolver_service_;
}

utils::result<std::unique_ptr<statement_impl>, utils::error>
connection::perform_prepare(const query_context &ctx) const {
  if (ctx.command != query_command::CreateTable &&
      (ctx.columns.empty() || has_unknown_columns(ctx.columns))) {
    if (const auto result = describe(ctx.table_name); result.is_ok()) {
      for (auto &col : ctx.columns) {
        const auto rit =
            std::find_if(std::begin(*result), std::end(*result), [&col, &ctx](const auto &value) {
              if (ctx.table_name.empty()) {
                return value.name() == col.name();
              }
              return value.name() == col.name() ||
                     ctx.table_name + "." + value.name() == col.name();
            });
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
} // namespace matador::query
