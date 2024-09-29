#ifndef QUERY_STATEMENT_IMPL_HPP
#define QUERY_STATEMENT_IMPL_HPP

#include "matador/sql/query_context.hpp"
#include "matador/sql/query_result_impl.hpp"

#include "matador/object/attribute_writer.hpp"
#include "matador/object/data_type_traits.hpp"

#include <memory>

namespace matador::sql {

class statement_impl
{
protected:
  explicit statement_impl(query_context query);

public:
  virtual ~statement_impl() = default;

  virtual size_t execute() = 0;
  virtual std::unique_ptr<query_result_impl> fetch() = 0;

  template < class Type >
  void bind(size_t pos, Type &val)
  {
    object::data_type_traits<Type>::bind_value(binder(), pos, val);
  }

  void bind(size_t pos, const char *value, size_t size);
  void bind(size_t pos, std::string &val, size_t size);

  virtual void reset() = 0;

  [[nodiscard]] const std::vector<std::string>& bind_vars() const;
  [[nodiscard]] bool is_valid_host_var(const std::string &host_var, size_t pos) const;

protected:
  virtual object::attribute_writer& binder() = 0;

protected:
  friend class statement;

  query_context query_;
};

}

#endif //QUERY_STATEMENT_IMPL_HPP
