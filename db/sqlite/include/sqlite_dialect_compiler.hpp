//
// Created by sascha on 17.06.16.
//

#ifndef OOS_SQLITE_DIALECT_COMPILER_HPP
#define OOS_SQLITE_DIALECT_COMPILER_HPP

#include "sql/basic_dialect_compiler.hpp"

namespace oos {

namespace sqlite {

class sqlite_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  sqlite_dialect_compiler();

  virtual void visit(const oos::detail::update &update1) override;
  virtual void visit(const oos::detail::remove &remove1) override;
  virtual void visit(const oos::columns &columns1) override;
  virtual void visit(const oos::detail::where &where1) override;
  virtual void visit(const oos::detail::basic_column_condition &condition) override;
  virtual void visit(const oos::detail::top &top1) override;

protected:
  virtual void on_compile_start() override;

private:
  bool is_update = false;
  bool is_delete = false;

  std::vector<std::shared_ptr<column>> columns_;
  std::vector<std::shared_ptr<detail::basic_column_condition>> column_conditions_;
  std::shared_ptr<detail::basic_condition> condition_;
};

}
}

#endif //OOS_SQLITE_DIALECT_COMPILER_HPP
