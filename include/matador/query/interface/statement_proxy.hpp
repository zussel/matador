#ifndef STATEMENT_PROXY_HPP
#define STATEMENT_PROXY_HPP

#include "matador/query/interface/statement_impl.hpp"

namespace matador::query {
class statement_proxy {
protected:
  explicit statement_proxy(std::unique_ptr<statement_impl>&& stmt);

public:
  virtual ~statement_proxy() = default;

  virtual utils::result<execute_result, utils::error> execute(value_writer& bindings) = 0;
  virtual utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch(value_writer& bindings) = 0;

  template<class Type>
  void bind(const Type &obj, value_writer& bindings) {
    statement_->bind_object(obj, bindings);
  }
  template<typename Type>
  void bind(size_t pos, Type &value, value_writer& bindings) {
    statement_->bind(pos, value, bindings);
  }
  template<typename Type>
  void bind(Type &value, value_writer& bindings) {
    statement_->bind(value, bindings);
  }
  void bind(size_t pos, const char *value, size_t size, value_writer& bindings) const;
  void bind(const char *value, size_t size, value_writer& bindings) const;
  void bind(size_t pos, std::string &val, size_t size, value_writer& bindings) const;
  void bind(std::string &val, size_t size, value_writer& bindings) const;

  void reset() const;
  [[nodiscard]] size_t bind_pos() const;

  [[nodiscard]] std::string sql() const;

  [[nodiscard]] std::unique_ptr<value_writer> create_binder() const;

protected:
  friend class statement;

  std::unique_ptr<statement_impl> statement_;
};


}

#endif //STATEMENT_PROXY_HPP
