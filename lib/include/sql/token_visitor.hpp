//
// Created by sascha on 6/1/16.
//

#ifndef OOS_TOKEN_VISITOR_HPP
#define OOS_TOKEN_VISITOR_HPP

namespace oos {

class sql;
struct column;
struct columns;

namespace detail {

struct select;
struct insert;
struct update;
struct remove;
struct typed_column;
struct identifier_column;
struct typed_varchar_column;
struct identifier_varchar_column;
struct basic_value_column;
struct values;
struct basic_value;
struct distinct;
struct set;
struct asc;
struct desc;
struct from;
struct where;
struct basic_condition;
struct basic_column_condition;
struct basic_in_condition;
struct create;
struct drop;
struct top;
struct as;
struct order_by;
struct group_by;
struct begin;
struct commit;
struct rollback;
struct query;

}

class token_visitor
{
public:
  virtual ~token_visitor() {}

  virtual void visit(const oos::detail::create &) = 0;
  virtual void visit(const oos::detail::drop &) = 0;
  virtual void visit(const oos::detail::select &) = 0;
  virtual void visit(const oos::detail::distinct &) = 0;
  virtual void visit(const oos::detail::update &) = 0;
  virtual void visit(const oos::detail::set &) = 0;
  virtual void visit(const oos::columns &) = 0;
  virtual void visit(const oos::column &) = 0;
  virtual void visit(const oos::detail::typed_column &) = 0;
  virtual void visit(const oos::detail::identifier_column &) = 0;
  virtual void visit(const oos::detail::typed_varchar_column &) = 0;
  virtual void visit(const oos::detail::identifier_varchar_column &) = 0;
  virtual void visit(const oos::detail::basic_value_column &) = 0;
  virtual void visit(const oos::detail::from &) = 0;
  virtual void visit(const oos::detail::where &) = 0;
  virtual void visit(const oos::detail::basic_condition &) = 0;
  virtual void visit(const oos::detail::basic_column_condition &) = 0;
  virtual void visit(const oos::detail::basic_in_condition &) = 0;
  virtual void visit(const oos::detail::order_by &) = 0;
  virtual void visit(const oos::detail::asc &) = 0;
  virtual void visit(const oos::detail::desc &) = 0;
  virtual void visit(const oos::detail::group_by &) = 0;
  virtual void visit(const oos::detail::insert &) = 0;
  virtual void visit(const oos::detail::values &) = 0;
  virtual void visit(const oos::detail::basic_value &) = 0;
  virtual void visit(const oos::detail::remove &) = 0;
  virtual void visit(const oos::detail::top &) = 0;
  virtual void visit(const oos::detail::as &) = 0;
  virtual void visit(const oos::detail::begin &) = 0;
  virtual void visit(const oos::detail::commit &) = 0;
  virtual void visit(const oos::detail::rollback &) = 0;
  virtual void visit(oos::detail::query &) = 0;
};

}
#endif //OOS_TOKEN_VISITOR_HPP
