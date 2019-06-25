//
// Created by sascha on 21.06.19.
//

#ifndef MATADOR_POSTGRESQL_DIALECT_COMPILER_HPP
#define MATADOR_POSTGRESQL_DIALECT_COMPILER_HPP

#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/dialect_token.hpp"

namespace matador {
namespace postgresql {

class postgresql_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  void visit(const matador::detail::select &select1) override;
  void visit(const matador::detail::update &update1) override;
  void visit(const matador::detail::remove &remove1) override;
  void visit(const matador::detail::tablename &tab) override;
  void visit(const matador::detail::from &from1) override;

  void visit(const matador::detail::where &where1) override;
  void visit(const matador::detail::top &limit) override;

protected:
  void on_compile_start() override;

private:
  bool is_update = false;

  std::string tablename_;
  token_list_t::iterator where_;
};

}
}
#endif //MATADOR_POSTGRESQL_DIALECT_COMPILER_HPP
