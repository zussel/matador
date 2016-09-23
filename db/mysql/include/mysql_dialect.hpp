//
// Created by sascha on 3/8/16.
//

#ifndef OOS_MYSQL_DIALECT_HPP
#define OOS_MYSQL_DIALECT_HPP

#include "sql/column.hpp"
#include "sql/basic_dialect.hpp"

namespace oos {
namespace mysql {

class mysql_dialect : public basic_dialect
{
public:
  mysql_dialect();
  const char* type_string(oos::data_type type) const;
  data_type string_type(const char *type) const;
};

}
}

#endif //OOS_MYSQL_DIALECT_HPP
