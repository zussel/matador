#ifndef OOS_MSSQL_DIALECT_LINKER_HPP
#define OOS_MSSQL_DIALECT_LINKER_HPP

#include "matador/sql/basic_dialect_linker.hpp"

namespace matador::mssql {

class mssql_dialect_linker : public detail::basic_dialect_linker
{
public:
  using detail::basic_dialect_linker::basic_dialect_linker;

  ~mssql_dialect_linker() override = default;

  void visit(const matador::detail::top &top) override;

};

}



#endif //OOS_MSSQL_DIALECT_LINKER_HPP
