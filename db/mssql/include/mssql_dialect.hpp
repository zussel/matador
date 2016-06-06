//
// Created by sascha on 3/9/16.
//

#ifndef OOS_MSSQL_DIALECT_HPP
#define OOS_MSSQL_DIALECT_HPP

#include "sql/basic_dialect.hpp"

namespace oos {
namespace mssql {

class mssql_dialect : public basic_dialect
{
public:
  mssql_dialect();
  const char* type_string(oos::data_type_t type) const;
  data_type_t string_type(const char *type) const;

  virtual void parse(token_list_t &tokens) const;

};

}
}

#endif //OOS_MSSQL_DIALECT_HPP
