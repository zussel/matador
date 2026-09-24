#include "matador/query/interface/statement_proxy.hpp"

namespace matador::query {
statement_proxy::statement_proxy(std::unique_ptr<statement_impl>&& stmt)
: statement_(std::move(stmt)){}

void statement_proxy::bind(const size_t pos, const char* value, const size_t size, value_writer& bindings) const {
  statement_->bind(pos, value, size, bindings);
}
void statement_proxy::bind(const char* value, const size_t size, value_writer& bindings) const {
  statement_->bind(value, size, bindings);
}
void statement_proxy::bind(const size_t pos, std::string& val, const size_t size, value_writer& bindings) const {
  statement_->bind(pos, val, size, bindings);
}
void statement_proxy::bind(std::string& val, const size_t size, value_writer& bindings) const {
  statement_->bind(val, size, bindings);
}

void statement_proxy::reset() const {
  statement_->reset();
}

size_t statement_proxy::bind_pos() const {
  return statement_->bind_pos();
}

std::string statement_proxy::sql() const {
  return statement_->query_.sql;
}

std::unique_ptr<value_writer> statement_proxy::create_binder() const {
  return statement_->create_binder();
}
}
