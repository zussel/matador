//
// Created by sascha on 6/13/16.
//

#ifndef OOS_MSSQL_DIALECT_COMPILER_HPP
#define OOS_MSSQL_DIALECT_COMPILER_HPP

#include <stack>

#include "sql/basic_dialect_compiler.hpp"

namespace oos {

namespace mssql {

class mssql_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  mssql_dialect_compiler();

  virtual void visit(const oos::detail::select &select1) override;
  virtual void visit(const oos::detail::update &update1) override;
  virtual void visit(const oos::detail::remove &remove1) override;
  virtual void visit(const oos::detail::top &top1) override;

protected:
  virtual void on_compile_start() override;

private:
  std::stack<token_list_t::iterator> commands_;
};

}
}
#endif //OOS_MSSQL_DIALECT_COMPILER_HPP
