//
// Created by sascha on 3/8/16.
//

#ifndef OOS_MYSQL_DIALECT_HPP
#define OOS_MYSQL_DIALECT_HPP

#include "matador/sql/column.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {
namespace mysql {

class mysql_dialect : public basic_dialect
{
public:
  mysql_dialect();
  const char* to_database_type_string(matador::data_type type) const override;
  database_type string_type(const char *type) const;

  dialect_traits::identifier identifier_escape_type() const override;
};

}
}

#endif //OOS_MYSQL_DIALECT_HPP
