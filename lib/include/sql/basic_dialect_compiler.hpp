//
// Created by sascha on 6/9/16.
//

#ifndef OOS_BASIC_DIALECT_COMPILER_HPP
#define OOS_BASIC_DIALECT_COMPILER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/token_visitor.hpp"
#include "sql/token_list.hpp"

#include <stack>

namespace oos {

class basic_dialect;

namespace detail {

struct build_info;

/// @cond OOS_DEV

class OOS_API basic_dialect_compiler : public token_visitor
{
public:
  void compile(basic_dialect &dialect);

  virtual void visit(const oos::detail::create &create1) override;
  virtual void visit(const oos::detail::drop &drop1) override;
  virtual void visit(const oos::detail::select &select1) override;
  virtual void visit(const oos::detail::distinct &distinct1) override;
  virtual void visit(const oos::detail::update &update1) override;
  virtual void visit(const oos::detail::tablename &table1) override;
  virtual void visit(const oos::detail::set &set1) override;
  virtual void visit(const oos::columns &columns1) override;
  virtual void visit(const oos::column &column1) override;
  virtual void visit(const oos::detail::typed_column &typedColumn) override;
  virtual void visit(const oos::detail::typed_identifier_column &identifierColumn) override;
  virtual void visit(const oos::detail::typed_varchar_column &varchar_column) override;
  virtual void visit(const oos::detail::identifier_varchar_column &varchar_column) override;
  virtual void visit(const oos::detail::basic_value_column &value_column) override;
  virtual void visit(const oos::detail::from &from1) override;
  virtual void visit(const oos::detail::where &where1) override;
  virtual void visit(const oos::detail::basic_condition &condition) override;
  virtual void visit(const oos::detail::basic_column_condition &condition) override;
  virtual void visit(const oos::detail::basic_in_condition &condition) override;
  virtual void visit(const oos::detail::order_by &by) override;
  virtual void visit(const oos::detail::asc &asc1) override;
  virtual void visit(const oos::detail::desc &desc1) override;
  virtual void visit(const oos::detail::group_by &by) override;
  virtual void visit(const oos::detail::insert &insert1) override;
  virtual void visit(const oos::detail::values &values1) override;
  virtual void visit(const oos::detail::basic_value &value) override;
  virtual void visit(const oos::detail::remove &remove1) override;
  virtual void visit(const oos::detail::top &top1) override;
  virtual void visit(const oos::detail::as &as1) override;
  virtual void visit(const oos::detail::begin &begin1) override;
  virtual void visit(const oos::detail::commit &commit1) override;
  virtual void visit(const oos::detail::rollback &rollback1) override;
  virtual void visit(oos::detail::query &query1) override;

protected:
  virtual void on_compile_start();
  virtual void on_compile_finish();

  basic_dialect& dialect() const;
  build_info& top() const;

private:
  friend class oos::basic_dialect;

  void dialect(basic_dialect *d);

  basic_dialect *dialect_;
};

/// @endcond

}

}

#endif //OOS_BASIC_DIALECT_COMPILER_HPP
