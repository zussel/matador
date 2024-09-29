#include "matador/sql/connection_impl.hpp"
#include "matador/sql/backend_provider.hpp"

namespace matador::sql {

connection_impl::connection_impl(const connection_info &info)
: info_(info)
, dialect_(backend_provider::instance().connection_dialect(info.type))
{}

const connection_info &connection_impl::info() const {
  return info_;
}

const class dialect & connection_impl::dialect() const {
  return dialect_;
}

}
