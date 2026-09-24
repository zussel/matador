#include "matador/query/interface/statement_impl.hpp"

namespace matador::query {

statement_impl::statement_impl(query_context query, const size_t start_bind_pos)
: query_(std::move(query))
, start_bind_pos_(start_bind_pos)
{}

void statement_impl::bind(const size_t pos, const char *value, const size_t size, value_writer& bindings) {
  current_bind_pos_ = pos;
  bind(value, size, bindings);
}

void statement_impl::bind(const char* value, const size_t size, value_writer& bindings) {
  data_type_traits<const char*>::bind_value(bindings, adjust_index(current_bind_pos_++), value, size);
}

void statement_impl::bind(const size_t pos, std::string &value, const size_t size, value_writer& bindings) {
  current_bind_pos_ = pos;
  bind(value, size, bindings);
}

void statement_impl::bind(std::string& value, const size_t size, value_writer& bindings) {
  data_type_traits<std::string>::bind_value(bindings, adjust_index(current_bind_pos_++), value, size);
}

void statement_impl::reset() {
  current_bind_pos_ = start_bind_pos_;
}

size_t statement_impl::bind_pos() const {
  return current_bind_pos_;
}

const std::vector<std::string> &statement_impl::bind_vars() const {
  return query_.bind_vars;
}

bool statement_impl::is_valid_host_var(const std::string &host_var, const size_t pos) const {
  const auto host_var_at_pos = bind_vars().at(pos);

  return host_var_at_pos == host_var;
}

size_t statement_impl::start_index() const {
    return start_bind_pos_;
}

size_t statement_impl::adjust_index(const size_t index) const {
    return index;
}

}