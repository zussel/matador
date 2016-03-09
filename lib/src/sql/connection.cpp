#include "sql/connection_factory.hpp"
#include "sql/connection.hpp"

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

const char *connection::type_string(data_type_t) const
{
  // Todo: re-implement
//  return impl_->type_string(type);
  return nullptr;
}
}