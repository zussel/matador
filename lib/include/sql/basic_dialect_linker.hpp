//
// Created by sascha on 14.08.16.
//

#ifndef OOS_BASIC_DIALECT_LINKER_HPP
#define OOS_BASIC_DIALECT_LINKER_HPP

#include "sql/token_visitor.hpp"
#include "sql/token.hpp"
#include "sql/token_list.hpp"

#include <string>
#include <stack>

namespace oos {

namespace detail {

struct build_info;

class basic_dialect_linker : public token_visitor
{
public:
  void link(const token_list_t &tokens, detail::build_info *buildinfo);

  std::string token_string(detail::token::t_token tok) const;

  virtual void visit(const oos::detail::create &);
  virtual void visit(const oos::detail::drop &);
  virtual void visit(const oos::detail::select &);
  virtual void visit(const oos::detail::distinct &);
  virtual void visit(const oos::detail::update &);
  virtual void visit(const oos::detail::tablename &table) override;
  virtual void visit(const oos::detail::set &);
  virtual void visit(const oos::detail::as &);
  virtual void visit(const oos::detail::top &);
  virtual void visit(const oos::detail::remove &);
  virtual void visit(const oos::detail::values &values);
  virtual void visit(const oos::detail::basic_value &);
  virtual void visit(const oos::detail::order_by &);
  virtual void visit(const oos::detail::asc &);
  virtual void visit(const oos::detail::desc &);
  virtual void visit(const oos::detail::group_by &);
  virtual void visit(const oos::detail::insert &);
  virtual void visit(const oos::detail::from &);
  virtual void visit(const oos::detail::where &);
  virtual void visit(const oos::detail::basic_condition &);
  virtual void visit(const oos::detail::basic_column_condition &);
  virtual void visit(const oos::detail::basic_in_condition &);
  virtual void visit(const oos::columns &);
  virtual void visit(const oos::column &);
  virtual void visit(const oos::detail::typed_column &);
  virtual void visit(const oos::detail::identifier_column &);
  virtual void visit(const oos::detail::typed_varchar_column &);
  virtual void visit(const oos::detail::identifier_varchar_column &);
  virtual void visit(const oos::detail::basic_value_column &);
  virtual void visit(const oos::detail::begin &);
  virtual void visit(const oos::detail::commit &);
  virtual void visit(const oos::detail::rollback &);
  virtual void visit(oos::detail::query &);

protected:
  std::stack<detail::build_info*> build_info_stack_;
};

}

}

#endif //OOS_BASIC_DIALECT_LINKER_HPP
