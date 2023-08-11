#ifndef OOS_BASIC_DIALECT_COMPILER_HPP
#define OOS_BASIC_DIALECT_COMPILER_HPP

#include "matador/sql/token_visitor.hpp"
#include "matador/sql/token_list.hpp"

#include <stack>

namespace matador {

class basic_dialect;

namespace detail {

struct build_info;

/// @cond MATADOR_DEV

class basic_dialect_compiler : public token_visitor
{
public:
  void compile(basic_dialect &dialect);

  void visit(const matador::detail::create &) override;
  void visit(const matador::detail::drop &) override;
  void visit(const matador::detail::select &) override;
  void visit(const matador::detail::distinct &) override;
  void visit(const matador::detail::update &) override;
  void visit(const matador::detail::tablename &) override;
  void visit(const matador::detail::set &) override;
  void visit(const matador::columns &) override;
  void visit(const matador::column &) override;
  void visit(const matador::detail::typed_column &) override;
  void visit(const matador::detail::typed_identifier_column &) override;
  void visit(const matador::detail::typed_varchar_column &) override;
  void visit(const matador::detail::identifier_varchar_column &) override;
  void visit(const matador::detail::value_column &) override;
  void visit(const matador::detail::from &) override;
  void visit(const matador::detail::where &) override;
  void visit(const matador::detail::basic_condition &) override;
  void visit(const matador::detail::basic_column_condition &) override;
  void visit(const matador::detail::basic_in_condition &) override;
  void visit(const matador::detail::order_by &) override;
  void visit(const matador::detail::asc &) override;
  void visit(const matador::detail::desc &) override;
  void visit(const matador::detail::group_by &) override;
  void visit(const matador::detail::insert &) override;
  void visit(const matador::detail::values &) override;
  void visit(const matador::value &) override;
  void visit(const matador::detail::remove &) override;
  void visit(const matador::detail::top &) override;
  void visit(const matador::detail::as &) override;
  void visit(const matador::detail::begin &) override;
  void visit(const matador::detail::commit &) override;
  void visit(const matador::detail::rollback &) override;
  void visit(matador::detail::query &) override;

protected:
  virtual void on_compile_start();
  virtual void on_compile_finish();

  basic_dialect& dialect() const;
  build_info& top() const;

private:
  friend class matador::basic_dialect;

  void dialect(basic_dialect *d);

  basic_dialect *dialect_;
};

/// @endcond

}

}

#endif //OOS_BASIC_DIALECT_COMPILER_HPP
