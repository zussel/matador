#include <matador/orm/identifier_column_resolver.hpp>

namespace matador {
namespace detail {

void identifier_column_resolver::on_primary_key(const char *id, std::string &, size_t /*size*/)
{
  col_.name(id);
}

}
}