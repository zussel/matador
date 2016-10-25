#include "sql/connection_factory.hpp"
#include "sql/connection.hpp"
#include "sql/column.hpp"
#include "sql/value.hpp"

namespace oos {

connection::connection() {}

connection::connection(const std::string &dns)
{
  parse_dns(dns);
  impl_.reset(create_connection(type_));
}

connection::connection(const connection &x)
  : type_(x.type_)
  , dns_(x.dns_)
{
  impl_.reset(create_connection(type_));

  if (x.is_open()) {
    open();
  }
}

connection::connection(connection &&x)
  : type_(std::move(x.type_))
  , dns_(std::move(x.dns_))
  , impl_(std::move(x.impl_))
{}

connection &connection::operator=(const connection &x)
{
  type_ = x.type_;
  dns_ = x.dns_;
  impl_.reset(create_connection(type_));

  if (x.is_open()) {
    open();
  }

  return *this;
}

connection &connection::operator=(connection &&x)
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

void connection::open(const std::string &dns)
{
  if (is_open()) {
    return;
  } else {
    parse_dns(dns);
    impl_->open(dns_);
  }
}

void connection::open()
{
  if (is_open()) {
    return;
  } else {
    if (impl_) {
      connection_factory::instance().destroy(type_, impl_.release());
    }
    impl_.reset(create_connection(type_));
    impl_->open(dns_);
  }
}

bool connection::is_open() const
{
  return impl_->is_open();
}

void connection::close()
{
  impl_->close();
}

void connection::begin()
{
  impl_->begin();
}

void connection::commit()
{
  impl_->commit();
}

void connection::rollback()
{
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

bool connection::exists(const std::string &tablename) const
{
  return impl_->exists(tablename);
}

std::vector<field> connection::describe(const std::string &table) const
{
  return impl_->describe(table);
}

basic_dialect *connection::dialect()
{
  return impl_->dialect();
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
    };
    // generate value by type
//    f.type();
//    std::shared_ptr<detail::basic_value> value(new null_value);
    std::shared_ptr<detail::basic_value> value(create_default_value(f.type()));
    prototype.set(f.name(), value);
//    prototype.set(f.name(), std::make_shared<null_value>());
  }
  // default value for count(*)
  if (prototype.has_column(oos::columns::count_all().name)) {
    std::shared_ptr<detail::basic_value> value(create_default_value(data_type::type_int));
    prototype.set(oos::columns::count_all().name, value);
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
      return make_value<oos::date>(date());
    case data_type::type_time:
      return make_value<oos::time>(oos::time());
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

void connection::parse_dns(const std::string &dns)
{
  std::string::size_type pos = dns.find(':');
  type_ = dns.substr(0, pos);
  dns_ = dns.substr(pos + 3);
}

}