//
// Created by sascha on 14.08.16.
//

#ifndef OOS_BASIC_DIALECT_LINKER_HPP
#define OOS_BASIC_DIALECT_LINKER_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/token_visitor.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/token_list.hpp"

#include <string>
#include <stack>

namespace matador {

class basic_dialect;

namespace detail {

struct build_info;

/// @cond MATADOR_DEV

class OOS_SQL_API basic_dialect_linker : public token_visitor
{
public:
  void link();

  std::string token_string(detail::token::t_token tok) const;

  virtual void visit(const matador::detail::create &) override;
  virtual void visit(const matador::detail::drop &) override;
  virtual void visit(const matador::detail::select &) override;
  virtual void visit(const matador::detail::distinct &) override;
  virtual void visit(const matador::detail::update &) override;
  virtual void visit(const matador::detail::tablename &table) override;
  virtual void visit(const matador::detail::set &) override;
  virtual void visit(const matador::detail::as &) override;
  virtual void visit(const matador::detail::top &) override;
  virtual void visit(const matador::detail::remove &) override;
  virtual void visit(const matador::detail::values &values) override;
  virtual void visit(const matador::detail::basic_value &) override;
  virtual void visit(const matador::detail::order_by &) override;
  virtual void visit(const matador::detail::asc &) override;
  virtual void visit(const matador::detail::desc &) override;
  virtual void visit(const matador::detail::group_by &) override;
  virtual void visit(const matador::detail::insert &) override;
  virtual void visit(const matador::detail::from &) override;
  virtual void visit(const matador::detail::where &) override;
  virtual void visit(const matador::detail::basic_condition &) override;
  virtual void visit(const matador::detail::basic_column_condition &) override;
  virtual void visit(const matador::detail::basic_in_condition &) override;
  virtual void visit(const matador::columns &) override;
  virtual void visit(const matador::column &) override;
  virtual void visit(const matador::detail::typed_column &) override;
  virtual void visit(const matador::detail::typed_identifier_column &) override;
  virtual void visit(const matador::detail::typed_varchar_column &) override;
  virtual void visit(const matador::detail::identifier_varchar_column &) override;
  virtual void visit(const matador::detail::basic_value_column &) override;
  virtual void visit(const matador::detail::begin &) override;
  virtual void visit(const matador::detail::commit &) override;
  virtual void visit(const matador::detail::rollback &) override;
  virtual void visit(matador::detail::query &) override;

protected:
  basic_dialect& dialect() const;
  build_info& top() const;

  void append_to_result(basic_dialect &dialect, const std::string &part);

private:
  friend class matador::basic_dialect;

  void dialect(basic_dialect *d);

  basic_dialect *dialect_;
};

/// @endcond

}

}

#endif //OOS_BASIC_DIALECT_LINKER_HPP
