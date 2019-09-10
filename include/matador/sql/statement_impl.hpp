//
// Created by sascha on 9/18/15.
//

#ifndef OOS_STATEMENT_IMPL_HPP
#define OOS_STATEMENT_IMPL_HPP

#include "matador/sql/result.hpp"
#include "matador/sql/parameter_binder.hpp"

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

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API statement_impl
{
public:
  statement_impl() = delete;
  statement_impl& operator=(statement_impl &&) = delete;
  statement_impl(statement_impl &&) = delete;

public:
  statement_impl(basic_dialect *dialect, const matador::sql &stmt);
  statement_impl(const statement_impl &) = default;
  statement_impl& operator=(const statement_impl &) = default;
  virtual ~statement_impl() = default;

  virtual void clear() = 0;

  virtual detail::result_impl* execute() = 0;

  virtual void reset() = 0;

  template < class T >
  size_t bind(T *o, size_t pos)
  {
    matador::parameter_binder<void> binder(pos, this->binder());
    return binder.bind(*o);
  }

  template < class T, class V >
  size_t bind(V &val, size_t pos)
  {
    T obj;
    // get column name at pos
    if (pos >= bind_vars().size()) {
      throw std::out_of_range("host index out of range");
    }

    host_var_ = bind_vars().at(pos);

    matador::parameter_binder<V> binder(host_var_, val, pos, this->binder());
    pos = binder.bind(obj);

    host_var_.clear();

    return pos;
  }

  std::string str() const;
  const std::vector<std::string>& bind_vars() const;
  const std::vector<std::string>& columns() const;

  void log(const std::string &stmt) const;

  void enable_log();
  void disable_log();
  bool is_log_enabled() const;

protected:
  size_t host_index = 0;
  std::string host_var_;

  virtual detail::parameter_binder_impl* binder() const = 0;

private:
  std::string sql_;
  std::vector<std::string> bind_vars_;
  std::vector<std::string> columns_;

  bool log_enabled_ = false;
};

/// @endcond

}
}
#endif //OOS_STATEMENT_IMPL_HPP
