#ifndef QUERY_STATEMENT_IMPL_HPP
#define QUERY_STATEMENT_IMPL_HPP

#include "matador/query/query_context.hpp"
#include "matador/query/value_writer.hpp"
#include "matador/query/internal/object_binder.hpp"

#include "matador/query/type_traits.hpp"
#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>

namespace matador::query {
struct execute_result;
class query_result_impl;

class statement_impl {
protected:
  explicit statement_impl(query_context query, size_t start_bind_pos);

public:
  virtual ~statement_impl() = default;

  virtual utils::result<execute_result, utils::error> execute(const value_writer& bindings) = 0;
  virtual utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch(const value_writer& bindings) = 0;

  template < class Type >
  void bind_object(Type &obj, value_writer& bindings) {
    object_binder object_binder_;
    object_binder_.reset(start_index());
    object_binder_.bind(obj, bindings);

    current_bind_pos_ = object_binder_.current_index();
  }

  template < class Type >
  void bind(const size_t pos, Type &val, value_writer& bindings) {
    current_bind_pos_ = pos;
    bind(val, bindings);
  }
  template < class Type >
  void bind(Type &val, value_writer& bindings) {
    data_type_traits<Type>::bind_value(bindings, adjust_index(current_bind_pos_++), val);
  }
  void bind(size_t pos, const char *value, size_t size, value_writer& bindings);
  void bind(const char *value, size_t size, value_writer& bindings);
  void bind(size_t pos, std::string &val, size_t size, value_writer& bindings);
  void bind(std::string &value, size_t size, value_writer& bindings);

  virtual void reset();

  [[nodiscard]] size_t bind_pos() const;

  [[nodiscard]] const std::vector<std::string>& bind_vars() const;
  [[nodiscard]] bool is_valid_host_var(const std::string &host_var, size_t pos) const;

protected:
  [[nodiscard]] virtual size_t start_index() const;
  [[nodiscard]] virtual size_t adjust_index(size_t index) const;
  [[nodiscard]] virtual std::unique_ptr<value_writer> create_binder() const = 0;

protected:
  friend class statement;
  friend class statement_proxy;

  query_context query_;
  size_t start_bind_pos_{0};
  size_t current_bind_pos_{0};
};

}

#endif //QUERY_STATEMENT_IMPL_HPP
