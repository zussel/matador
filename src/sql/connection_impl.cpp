#include "matador/sql/connection_impl.hpp"

namespace matador::sql {

connection_impl::connection_impl(const connection_info &info)
: info_(info){}

const connection_info &connection_impl::info() const {
  return info_;
}

}