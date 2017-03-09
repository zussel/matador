//
// Created by sascha on 6/1/16.
//

#ifndef OOS_TOKEN_VISITOR_HPP
#define OOS_TOKEN_VISITOR_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

namespace matador {

class sql;
struct column;
struct columns;

namespace detail {

struct select;
struct insert;
struct update;
struct tablename;
struct remove;
struct typed_column;
struct typed_identifier_column;
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
class basic_condition;
class basic_column_condition;
class basic_in_condition;
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

/// @cond MATADOR_DEV

class OOS_SQL_API token_visitor
{
public:
  virtual ~token_visitor() {}

  virtual void visit(const matador::detail::create &) = 0;
  virtual void visit(const matador::detail::drop &) = 0;
  virtual void visit(const matador::detail::select &) = 0;
  virtual void visit(const matador::detail::distinct &) = 0;
  virtual void visit(const matador::detail::update &) = 0;
  virtual void visit(const matador::detail::tablename &) = 0;
  virtual void visit(const matador::detail::set &) = 0;
  virtual void visit(const matador::columns &) = 0;
  virtual void visit(const matador::column &) = 0;
  virtual void visit(const matador::detail::typed_column &) = 0;
  virtual void visit(const matador::detail::typed_identifier_column &) = 0;
  virtual void visit(const matador::detail::typed_varchar_column &) = 0;
  virtual void visit(const matador::detail::identifier_varchar_column &) = 0;
  virtual void visit(const matador::detail::basic_value_column &) = 0;
  virtual void visit(const matador::detail::from &) = 0;
  virtual void visit(const matador::detail::where &) = 0;
  virtual void visit(const matador::detail::basic_condition &) = 0;
  virtual void visit(const matador::detail::basic_column_condition &) = 0;
  virtual void visit(const matador::detail::basic_in_condition &) = 0;
  virtual void visit(const matador::detail::order_by &) = 0;
  virtual void visit(const matador::detail::asc &) = 0;
  virtual void visit(const matador::detail::desc &) = 0;
  virtual void visit(const matador::detail::group_by &) = 0;
  virtual void visit(const matador::detail::insert &) = 0;
  virtual void visit(const matador::detail::values &) = 0;
  virtual void visit(const matador::detail::basic_value &) = 0;
  virtual void visit(const matador::detail::remove &) = 0;
  virtual void visit(const matador::detail::top &) = 0;
  virtual void visit(const matador::detail::as &) = 0;
  virtual void visit(const matador::detail::begin &) = 0;
  virtual void visit(const matador::detail::commit &) = 0;
  virtual void visit(const matador::detail::rollback &) = 0;
  virtual void visit(matador::detail::query &) = 0;
};

/// @endcond

}
#endif //OOS_TOKEN_VISITOR_HPP
