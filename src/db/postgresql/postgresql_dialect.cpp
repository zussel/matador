//
// Created by sascha on 24.05.19.
//

#include "matador/db/postgresql/postgresql_dialect.hpp"

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect_linker.hpp"

namespace matador {
namespace postgresql {

postgresql_dialect::postgresql_dialect()
: basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{

}

const char *postgresql_dialect::type_string(matador::data_type type) const
{
  switch (type) {
    default:
    {
      std::stringstream msg;
      msg << "postgresql sql: unknown type xxx [" << (int)type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

matador::data_type postgresql_dialect::string_type(const char *type) const
{
  return data_type::type_bool;
}

matador::dialect_traits::identifier postgresql_dialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_BOTH_SAME;
}

}
}
