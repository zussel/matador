//
// Created by sascha on 3/3/16.
//

#include <sql/basic_dialect.hpp>

#ifndef OOS_SQLITE_DIALECT_HPP
#define OOS_SQLITE_DIALECT_HPP

namespace oos {
namespace sqlite {

class sqlite_dialect : public basic_dialect
{
public:
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
  virtual std::string compile(const oos::detail::varchar_column &column);
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
};
}
}
#endif //OOS_SQLITE_DIALECT_HPP
