//
// Created by sascha on 19.08.16.
//

#ifndef OOS_MSSQL_DIALECT_LINKER_HPP
#define OOS_MSSQL_DIALECT_LINKER_HPP

#include "oos/sql/basic_dialect_linker.hpp"

namespace oos {

namespace mssql {

class mssql_dialect_linker : public detail::basic_dialect_linker
{
public:
  virtual ~mssql_dialect_linker() {}

  virtual void visit(const oos::detail::top &top) override;

};

}

}

#endif //OOS_MSSQL_DIALECT_LINKER_HPP
