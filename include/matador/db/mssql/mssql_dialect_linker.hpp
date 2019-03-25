//
// Created by sascha on 19.08.16.
//

#ifndef OOS_MSSQL_DIALECT_LINKER_HPP
#define OOS_MSSQL_DIALECT_LINKER_HPP

#include "matador/sql/basic_dialect_linker.hpp"

namespace matador {

namespace mssql {

class mssql_dialect_linker : public detail::basic_dialect_linker
{
public:
  ~mssql_dialect_linker() override = default;

  void visit(const matador::detail::top &top) override;

};

}

}

#endif //OOS_MSSQL_DIALECT_LINKER_HPP
