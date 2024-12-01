#include "matador/sql/statement_impl.hpp"

namespace matador::sql {

statement_impl::statement_impl(query_context query)
: query_(std::move(query))
{}

void statement_impl::bind(const size_t pos, const char *value, const size_t size)
{
  utils::data_type_traits<const char*>::bind_value(binder(), adjust_index(pos), value, size);
}

void statement_impl::bind(const size_t pos, std::string &val, const size_t size)
{
  utils::data_type_traits<std::string>::bind_value(binder(), adjust_index(pos), val, size);
}

const std::vector<std::string> &statement_impl::bind_vars() const
{
  return query_.bind_vars;
}

bool statement_impl::is_valid_host_var(const std::string &host_var, const size_t pos) const
{
  const auto host_var_at_pos = bind_vars().at(pos);

  return host_var_at_pos == host_var;
}

size_t statement_impl::start_index() const
{
    return 0;
}

size_t statement_impl::adjust_index( size_t index ) const
{
    return index;
}

}