#ifndef OOS_BASIC_DIALECT_LINKER_HPP
#define OOS_BASIC_DIALECT_LINKER_HPP

#include "matador/sql/token_visitor.hpp"
#include "matador/sql/value_processor.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/token_list.hpp"

#include <string>
#include <stack>

namespace matador {

class basic_dialect;

namespace detail {

struct build_info;

/// @cond MATADOR_DEV

class basic_dialect_linker : public token_visitor
{
public:
  void link();

  std::string token_string(detail::token::t_token tok) const;

  void visit(const matador::detail::create &) override;
  void visit(const matador::detail::drop &) override;
  void visit(const matador::detail::select &) override;
  void visit(const matador::detail::distinct &) override;
  void visit(const matador::detail::update &) override;
  void visit(const matador::detail::tablename &table) override;
  void visit(const matador::detail::set &) override;
  void visit(const matador::detail::as &) override;
  void visit(const matador::detail::top &) override;
  void visit(const matador::detail::remove &) override;
  void visit(const matador::detail::values &values) override;
  void visit(const matador::value &) override;
  void visit(const matador::detail::order_by &) override;
  void visit(const matador::detail::asc &) override;
  void visit(const matador::detail::desc &) override;
  void visit(const matador::detail::group_by &) override;
  void visit(const matador::detail::insert &) override;
  void visit(const matador::detail::from &) override;
  void visit(const matador::detail::where &) override;
  void visit(const matador::detail::basic_condition &) override;
  void visit(const matador::detail::basic_column_condition &) override;
  void visit(const matador::detail::basic_in_condition &) override;
  void visit(const matador::columns &) override;
  void visit(const matador::column &) override;
  void visit(const matador::detail::begin &) override;
  void visit(const matador::detail::commit &) override;
  void visit(const matador::detail::rollback &) override;
  void visit(matador::detail::query &) override;

protected:
  basic_dialect& dialect() const;
  build_info& top() const;

  static void append_to_result(basic_dialect &dialect, const std::string &part);

private:
  friend class matador::basic_dialect;

  void dialect(basic_dialect *d);

  basic_dialect *dialect_;
  detail::value_to_string_processor value_to_string_visitor_;
};

/// @endcond

}

}

#endif //OOS_BASIC_DIALECT_LINKER_HPP
