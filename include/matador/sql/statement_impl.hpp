#ifndef OOS_STATEMENT_IMPL_HPP
#define OOS_STATEMENT_IMPL_HPP

#include "matador/sql/result.hpp"
#include "matador/sql/parameter_binder.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV

class statement_impl
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
    reset();
    matador::parameter_binder<void> binder(pos, this->binder());
    return binder.bind(*o);
  }

  template < class T, class V >
  size_t bind(V &val, long size, size_t pos)
  {
    T obj;
    if (pos >= bind_vars().size()) {
      throw std::out_of_range("host index out of range");
    }

    // get column name at pos
    host_var_ = bind_vars().at(pos);

//    this->binder()->bind(val, pos);
    matador::parameter_binder<typename std::remove_const<V>::type> binder(host_var_, val, size, pos, this->binder());
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
//  virtual size_t normalize_position(size_t pos) const = 0;
//  virtual bool is_valid_host_var_position(size_t pos) const = 0;
//  virtual std::string bind_var_at(size_t pos) const = 0;

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
