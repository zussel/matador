#include "matador/sql/connection_factory.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/value.hpp"

namespace matador {

connection::connection(const std::string &dns)
{
  parse_dns(dns);
  impl_.reset(create_connection(type_));
}

connection::connection(const connection &x)
  : type_(x.type_)
  , dns_(x.dns_)
{
  init_from_foreign_connection(x);
}

connection::connection(connection &&x) noexcept
  : type_(std::move(x.type_))
  , dns_(std::move(x.dns_))
  , impl_(std::move(x.impl_))
{}

connection &connection::operator=(const connection &x)
{
  type_ = x.type_;
  dns_ = x.dns_;

  init_from_foreign_connection(x);

  return *this;
}

connection &connection::operator=(connection &&x) noexcept
{
  type_ = std::move(x.type_);
  dns_ = std::move(x.dns_);
  impl_ = std::move(x.impl_);
  return *this;
}

connection::~connection()
{
  if (!impl_) {
    return;
  }
  impl_->close();
  connection_factory::instance().destroy(type_, impl_.release());
}

void connection::connect(const std::string &dns)
{
  if (is_connected()) {
    return;
  } else {
    parse_dns(dns);
    impl_->open(dns_);
  }
}

void connection::connect()
{
  if (is_connected()) {
    return;
  } else {
    if (!impl_) {
      connection_factory::instance().destroy(type_, impl_.release());
      impl_.reset(create_connection(type_));
    }
    impl_->open(dns_);
  }
}

void connection::reconnect()
{
  if (is_connected()) {
    impl_->close();
  }
  impl_->open(dns_);
}

bool connection::is_connected() const
{
  return impl_->is_open();
}

void connection::disconnect()
{
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
  return type_;
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
  return !type_.empty() && !dns_.empty();
}

detail::basic_value* create_default_value(data_type type);

void connection::prepare_prototype_row(row &prototype, const std::string &tablename)
{
  if (!impl_->exists(tablename)) {
    return;
  }
  auto fields = impl_->describe(tablename);
  for (auto &&f : fields) {
    if (!prototype.has_column(f.name())) {
      continue;
    }
    // generate value by type
    std::shared_ptr<detail::basic_value> value(create_default_value(f.type()));
    prototype.set(f.name(), value);
  }
  // default value for count(*)
  if (prototype.has_column(matador::columns::count_all().name)) {
    std::shared_ptr<detail::basic_value> value(create_default_value(data_type::type_int));
    prototype.set(matador::columns::count_all().name, value);
  }
}

detail::basic_value* create_default_value(data_type type)
{
  switch (type) {
    case data_type::type_char:
      return make_value((char)0);
    case data_type::type_short:
      return make_value<short>(0);
    case data_type::type_int:
      return make_value<int>(0);
    case data_type::type_long:
      return make_value<long>(0);
    case data_type::type_unsigned_char:
      return make_value<unsigned char>(0);
    case data_type::type_unsigned_short:
      return make_value<unsigned short>(0);
    case data_type::type_unsigned_int:
      return make_value<unsigned int>(0);
    case data_type::type_unsigned_long:
      return make_value<unsigned long>(0);
    case data_type::type_float:
      return make_value<float>(0);
    case data_type::type_double:
      return make_value<double>(0);
    case data_type::type_char_pointer:
      return new value<char*>((char*)nullptr, 0UL);
    case data_type::type_text:
      return make_value<std::string>("");
    case data_type::type_date:
      return make_value<matador::date>(date());
    case data_type::type_time:
      return make_value<matador::time>(matador::time());
    case data_type::type_varchar:
      return make_value<std::string>("");
    default:
      return new null_value;
  }
}

connection_impl *connection::create_connection(const std::string &type) const
{
  // try to create sql implementation
  return connection_factory::instance().create(type);
}

void connection::init_from_foreign_connection(const connection &foreign_connection)
{
  if (is_valid()) {
    impl_.reset(create_connection(type_));

    if (foreign_connection.is_connected()) {
      connect();
    }
  }
}

void connection::parse_dns(const std::string &dns)
{
  std::string::size_type pos = dns.find(':');
  type_ = dns.substr(0, pos);
  dns_ = dns.substr(pos + 3);
}

void connection::log(const std::string &msg) const
{
  if (impl_->is_log_enabled()) {
    std::cout << "SQL: " << msg << "\n";
  }
}

void connection::log_token(detail::token::t_token tok)
{
  if (impl_->is_log_enabled()) {
    std::cout << "SQL: " << dialect()->token_at(tok) << "\n";
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