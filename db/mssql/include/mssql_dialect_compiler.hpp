//
// Created by sascha on 6/13/16.
//

#ifndef OOS_MSSQL_DIALECT_COMPILER_HPP
#define OOS_MSSQL_DIALECT_COMPILER_HPP

#include "sql/basic_dialect_compiler.hpp"

namespace oos {

namespace mssql {

class mssql_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  mssql_dialect_compiler();
};

}
}
#endif //OOS_MSSQL_DIALECT_COMPILER_HPP
