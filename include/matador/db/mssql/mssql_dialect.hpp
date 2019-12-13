//
// Created by sascha on 3/9/16.
//

#ifndef OOS_MSSQL_DIALECT_HPP
#define OOS_MSSQL_DIALECT_HPP

#include "matador/sql/basic_dialect.hpp"

namespace matador {
namespace mssql {

class mssql_dialect : public basic_dialect
{
public:
  mssql_dialect();
  const char* to_database_type_string(matador::data_type type) const override;
  database_type string_type(const char *type) const;

  dialect_traits::identifier identifier_escape_type() const override;
};

}
}

#endif //OOS_MSSQL_DIALECT_HPP
