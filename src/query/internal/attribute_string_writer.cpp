#include "matador/query/internal/attribute_string_writer.hpp"

#include "matador/query/query_utils.hpp"
#include "matador/query/dialect.hpp"

#include "matador/utils/convert.hpp"
#include "matador/utils/string.hpp"

namespace matador::query {
attribute_string_writer::attribute_string_writer(const class dialect &d,
                                                 const std::optional<std::reference_wrapper<const
                                                 connection_impl> > conn)
: dialect_(d)
, conn_(conn) {}

const dialect &attribute_string_writer::dialect() const {
  return dialect_;
}

void attribute_string_writer::write_value(size_t /*pos*/, const int8_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const int16_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const int32_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const int64_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const uint8_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const uint16_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const uint32_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const uint64_t &x) {
  result_ = std::to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const bool &x) {
  //  result_ = "'" + conn_->dialect().prepare_literal(x ? "true" : "false") + "'";
  result_ = dialect_.to_string(x);
}

void attribute_string_writer::write_value(size_t /*pos*/, const float &x) {
  if (const auto res = utils::to<std::string>(x); !res.is_error()) {
    result_ = *res;
  }
}

void attribute_string_writer::write_value(size_t /*pos*/, const double &x) {
  if (const auto res = utils::to<std::string>(x); !res.is_error()) {
    result_ = *res;
  }
}

void attribute_string_writer::write_value(size_t /*pos*/, const utils::date_type_t &x) {
  result_ = "'" + prepare_literal(utils::to_string(x), dialect_) + "'";
}

void attribute_string_writer::write_value(size_t /*pos*/, const utils::time_type_t &x) {
  result_ = "'" + prepare_literal(utils::to_string(x), dialect_) + "'";
}

void attribute_string_writer::write_value(size_t /*pos*/, const utils::timestamp_type_t &/*x*/) {
}

void attribute_string_writer::write_value(size_t pos, const char *x) {
  write_value(pos, std::string(x));
}

void attribute_string_writer::write_value(size_t pos, const char *x, size_t /*size*/) {
  write_value(pos, std::string(x));
}

void attribute_string_writer::write_value(size_t /*pos*/, const std::string &x) {
  result_ = "'" + prepare_literal(x, dialect_) + "'";
}

void attribute_string_writer::write_value(size_t /*pos*/, const std::string &x, size_t /*size*/) {
  result_ = "'" + prepare_literal(x, dialect_) + "'";
}

void attribute_string_writer::write_value(size_t /*pos*/, const utils::blob_type_t &x) {
  // "This is a binary Data string" as binary data:
  // MySQL:    X'5468697320697320612062616E617279204461746120737472696E67'
  // Postgres: '\\x5468697320697320612062616E617279204461746120737472696E67'
  // MSSQL:    0x5468697320697320612062616E617279204461746120737472696E67
  // Sqlite:   X'5468697320697320612062616E617279204461746120737472696E67'
  if (conn_.has_value()) {
    result_ = dialect_.token_at(dialect_token::BeginBinaryData) + conn_.value().get().to_escaped_string(x) +
              dialect_.token_at(dialect_token::EndBinaryData);
  } else {
    result_ = dialect_.token_at(dialect_token::BeginBinaryData) + dialect_.to_escaped_string(x) + dialect_.
              token_at(dialect_token::EndBinaryData);
  }
}

void attribute_string_writer::write_value(size_t /*pos*/, const column_value &/*x*/, size_t /*size*/) {
}
}
