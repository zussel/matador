#include "sql/connection_factory.hpp"
#include "sql/connection.hpp"
#include "sql/value.hpp"

namespace oos {

connection::connection(const std::string &dns)
{
  // parse dbstring
  std::string::size_type pos = dns.find(':');
  type_ = dns.substr(0, pos);
  dns_ = dns.substr(pos + 3);

  // get driver factory singleton
  connection_factory &df = connection_factory::instance();

  // try to create sql implementation
  impl_ = df.create(type_);
}

connection::~connection()
{
  impl_->close();
  connection_factory::instance().destroy(type_, impl_);
}

void connection::open()
{
  if (is_open()) {
    return;
  } else {
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

const char *connection::type_string(data_type_t) const
{
  // Todo: re-implement
//  return impl_->type_string(type);
  return nullptr;
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

void connection::prepare_prototype_row(row &prototype, const std::string &tablename)
{
  auto fields = impl_->describe(tablename);
  for (auto &&f : fields) {
    if (!prototype.has_column(f.name())) {
      continue;
    };
    // generate value by type
//    f.type();
    std::shared_ptr<detail::basic_value> value(new null_value);
    prototype.set(f.name(), value);
//    prototype.set(f.name(), std::make_shared<null_value>());
  }
}

detail::basic_value* create_default_value(data_type_t type)
{
  switch (type) {
    case type_char:
      return make_value((char)0);
    case type_short:
      return make_value<short>(0);
    case type_int:
      return make_value<int>(0);
    case type_long:
      return make_value<long>(0);
    case type_unsigned_char:
      return make_value<unsigned char>(0);
    case type_unsigned_short:
      return make_value<unsigned short>(0);
    case type_unsigned_int:
      return make_value<unsigned int>(0);
    case type_unsigned_long:
      return make_value<unsigned long>(0);
    case type_float:
      return make_value<float>(0);
    case type_double:
      return make_value<double>(0);
    case type_char_pointer:
      return new value<char*>((char*)nullptr, 0UL);
    case type_text:
      return make_value<std::string>("");
    case type_date:
      return make_value<oos::date>(date());
    case type_time:
      return make_value<oos::time>(oos::time());
    case type_varchar:
      return make_value<std::string>("");
    default:
      return new null_value;
  }
}

}