#include "matador/sql/connection_factory.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/columns.hpp"
#include "matador/sql/value.hpp"

namespace matador {

connection::connection(const std::string &dns)
{
  initialize_connection_info(dns);
}

connection::connection(std::shared_ptr<basic_sql_logger> sqllogger)
  : logger_(std::move(sqllogger))
{}

connection::connection(const std::string &dns, std::shared_ptr<basic_sql_logger> sql_logger)
  : logger_(std::move(sql_logger))
{
  initialize_connection_info(dns);
}

connection::connection(const connection &x)
  : connection_info_(x.connection_info_)
{
  init_from_foreign_connection(x);
}

connection::connection(connection &&x) noexcept
  : connection_info_(std::move(x.connection_info_))
  , impl_(std::move(x.impl_))
{}

connection &connection::operator=(const connection &x)
{
  if (this == &x) {
    return *this;
  }
  connection_info_ = x.connection_info_;

  init_from_foreign_connection(x);

  return *this;
}

connection &connection::operator=(connection &&x) noexcept
{
  connection_info_ = std::move(x.connection_info_);
  impl_ = std::move(x.impl_);
  return *this;
}

connection::~connection()
{
  if (!impl_) {
    return;
  }
  impl_->close();
  connection_factory::instance().destroy(connection_info_.type, impl_.release());
}

void connection::connect(const std::string &dns)
{
  if (is_connected()) {
    return;
  } else {
    initialize_connection_info(dns);
    logger_->on_connect();
    impl_->open(connection_info_);
  }
}

void connection::connect()
{
  if (is_connected()) {
    return;
  } else {
    if (!impl_) {
      connection_factory::instance().destroy(connection_info_.type, impl_.release());
      impl_.reset(create_connection(connection_info_.type));
    }
    logger_->on_connect();
    impl_->open(connection_info_);
  }
}

void connection::reconnect()
{
  if (is_connected()) {
    logger_->on_close();
    impl_->close();
  }
  logger_->on_connect();
  impl_->open(connection_info_);
}

bool connection::is_connected() const
{
  return impl_->is_open();
}

void connection::disconnect()
{
  logger_->on_close();
  impl_->close();
}

void connection::begin()
{
  log_token(detail::token::BEGIN);
  impl_->begin();
}

void connection::commit()
{
  log_token(detail::token::COMMIT);
  impl_->commit();
}

void connection::rollback()
{
  log_token(detail::token::ROLLBACK);
  impl_->rollback();
}

std::string connection::type() const
{
  return connection_info_.type;
}

std::string connection::version() const
{
  return "";
}

bool connection::exists(const std::string &table_name) const
{
  return impl_->exists(table_name);
}

std::vector<field> connection::describe(const std::string &table) const
{
  return impl_->describe(table);
}

basic_dialect *connection::dialect()
{
  return impl_->dialect();
}

bool connection::is_valid() const
{
  return !connection_info_.type.empty() && !connection_info_.database.empty();
}

value* create_default_value(database_type type);

void connection::initialize_connection_info(const std::string &dns)
{
  connection_info_ = connection_info::parse(dns);
  impl_.reset(create_connection(connection_info_.type));
  if (connection_info_.port == 0) {
    connection_info_.port = impl_->default_port();
  }
}

void connection::prepare_prototype_row(row &prototype, const std::string &table_name)
{
  if (!impl_->exists(table_name)) {
    return;
  }
  auto fields = impl_->describe(table_name);
  for (const auto &f : fields) {
    if (!prototype.has_column(f.name())) {
      continue;
    }
    // generate value by type
    std::shared_ptr<value> value(create_default_value(f.type()));
    prototype.set(f.name(), value);
  }
  // default value for count(*)
  if (prototype.has_column(matador::columns::count_all().name)) {
    std::shared_ptr<value> value(create_default_value(database_type::type_int));
    prototype.set(matador::columns::count_all().name, value);
  }
}

value* create_default_value(database_type type)
{
  switch (type) {
    case database_type::type_char:
      return make_value((char)0);
    case database_type::type_smallint:
      return make_value<short>(0);
    case database_type::type_int:
      return make_value<int>(0);
    case database_type::type_bigint:
      return make_value<long long>(0);
    case database_type::type_bool:
      return make_value<bool>(false);
    case database_type::type_float:
      return make_value<float>(0);
    case database_type::type_double:
      return make_value<double>(0);
    case database_type::type_char_pointer:
      return new value((char*)nullptr/*, 0UL*/);
    case database_type::type_text:
      return make_value<std::string>("");
    case database_type::type_date:
      return make_value<matador::date>(date());
    case database_type::type_time:
      return make_value<matador::time>(matador::time());
    case database_type::type_varchar:
      return make_value<std::string>("");
    default:
      return new value();
  }
}

connection_impl *connection::create_connection(const std::string &type)
{
  // try to create sql implementation
  return connection_factory::instance().create(type);
}

void connection::init_from_foreign_connection(const connection &foreign_connection)
{
  if (is_valid()) {
    impl_.reset(create_connection(connection_info_.type));

    if (foreign_connection.is_connected()) {
      connect();
    }
  }
}

void connection::log_token(detail::token::t_token tok)
{
  if (impl_->is_log_enabled()) {
    logger_->on_execute(dialect()->token_at(tok));
  }
}

void connection::enable_log()
{
  impl_->enable_log();
}

void connection::disable_log()
{
  impl_->disable_log();
}

bool connection::is_log_enabled() const
{
  return impl_->is_log_enabled();
}

}