//
// Created by sascha on 3/8/16.
//

#ifndef OOS_MYSQL_DIALECT_HPP
#define OOS_MYSQL_DIALECT_HPP

#include "sql/basic_dialect.hpp"

namespace oos {
namespace mysql {

class mysql_dialect : public basic_dialect
{
public:
  mysql_dialect();
  const char* type_string(oos::data_type_t type) const;

  virtual std::string compile(const oos::detail::create &create);
  virtual std::string compile(const oos::detail::drop &drop);
  virtual std::string compile(const oos::detail::select &select1);
  virtual std::string compile(const oos::detail::distinct &distinct);
  virtual std::string compile(const oos::detail::update &update);
  virtual std::string compile(const oos::detail::set &set);
  virtual std::string compile(const oos::detail::columns &columns);
  virtual std::string compile(const oos::column &column);
  virtual std::string compile(const oos::detail::typed_column &column);
  virtual std::string compile(const oos::detail::identifier_column &column);
  virtual std::string compile(const oos::detail::typed_varchar_column &column);
  virtual std::string compile(const oos::detail::from &from);
  virtual std::string compile(const oos::detail::where &where);
  virtual std::string compile(const oos::detail::basic_condition &token);
  virtual std::string compile(const oos::detail::order_by &by);
  virtual std::string compile(const oos::detail::asc &asc);
  virtual std::string compile(const oos::detail::desc &desc);
  virtual std::string compile(const oos::detail::group_by &by);
  virtual std::string compile(const oos::detail::insert &insert);
  virtual std::string compile(const oos::detail::values &values);
  virtual std::string compile(const oos::detail::basic_value &value);
  virtual std::string compile(const oos::detail::remove &remove1);
  virtual std::string compile(const oos::detail::top &limit);
  virtual std::string compile(const oos::detail::begin &bgn);
  virtual std::string compile(const oos::detail::commit &cmmt);
  virtual std::string compile(const oos::detail::rollback &rllbck);

  void reset() { bind_count_ = 0; column_count_ = 0; };
  int bind_count() const { return bind_count_; }
  int column_count() const { return column_count_; }

private:
  int bind_count_ = 0;
  int column_count_ = 0;
};

}
}

#endif //OOS_MYSQL_DIALECT_HPP
