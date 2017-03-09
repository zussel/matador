//
// Created by sascha on 6/13/16.
//

#ifndef OOS_MSSQL_DIALECT_COMPILER_HPP
#define OOS_MSSQL_DIALECT_COMPILER_HPP

#include <stack>

#include "matador/sql/basic_dialect_compiler.hpp"

namespace matador {

namespace mssql {

class mssql_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  mssql_dialect_compiler();

  virtual void visit(const matador::detail::select &select1) override;
  virtual void visit(const matador::detail::update &update1) override;
  virtual void visit(const matador::detail::remove &remove1) override;
  virtual void visit(const matador::detail::top &top1) override;

protected:
  virtual void on_compile_start() override;

private:
  std::stack<token_list_t::iterator> commands_;
};

}
}
#endif //OOS_MSSQL_DIALECT_COMPILER_HPP
