#include "matador/query/internal/identifier_statement_binder.hpp"

#include "matador/query/statement.hpp"

namespace matador::query {
identifier_statement_binder::identifier_statement_binder(statement &stmt, const size_t index)
: stmt_(stmt)
, index_(index) {}

void identifier_statement_binder::bind(const identifier &id) {
  id.serialize(*this);
}

void identifier_statement_binder::serialize(int8_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(int16_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(int32_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(int64_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(uint8_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(uint16_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(uint32_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(uint64_t &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(std::string &value, const column_options &) {
  stmt_.bind(index_, value);
}

void identifier_statement_binder::serialize(utils::null_type_t &/*value*/, const column_options &) {
}
}
