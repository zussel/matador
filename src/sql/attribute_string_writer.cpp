#include "matador/sql/attribute_string_writer.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/dialect.hpp"

#include "matador/utils/string.hpp"

namespace matador::sql {

attribute_string_writer::attribute_string_writer(const connection_impl& conn)
: conn_(&conn) {}

const sql::dialect& attribute_string_writer::dialect() const {
    return conn_->dialect();
}

void attribute_string_writer::write_value( size_t pos, const char& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const short& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const int& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const long& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const long long& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const unsigned char& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const unsigned short& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const unsigned& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const unsigned long& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const unsigned long long& x ) {
    result_ = std::to_string(x) ;
}

void attribute_string_writer::write_value( size_t pos, const bool& x ) {
  result_ = "'" + conn_->dialect().prepare_literal(x ? "true" : "false") + "'";
}

void attribute_string_writer::write_value( size_t pos, const float& x ) {
    utils::convert(result_, x);
}

void attribute_string_writer::write_value( size_t pos, const double& x ) {
    utils::convert(result_, x);
}

void attribute_string_writer::write_value( size_t pos, const time& x ) {
    result_ = "'" + conn_->dialect().prepare_literal(utils::to_string(x, "%FT%T.%f")) + "'";
    std::cout << "write time value: " << result_ << " (fraction: " << x.get_time_info().milliseconds << ")\n";
}

void attribute_string_writer::write_value( size_t pos, const date& x ) {
  result_ = "'" + conn_->dialect().prepare_literal(utils::to_string(x)) + "'";
}

void attribute_string_writer::write_value( size_t pos, const char* x ) {
  write_value( pos, std::string(x) );
}

void attribute_string_writer::write_value( size_t pos, const char* x, size_t size ) {
  write_value( pos, std::string(x) );
}

void attribute_string_writer::write_value( size_t pos, const std::string& x ) {
  result_ = "'" + conn_->dialect().prepare_literal(x) + "'";
}

void attribute_string_writer::write_value( size_t pos, const std::string& x, size_t size ) {
  result_ = "'" + conn_->dialect().prepare_literal(x) + "'";
}

void attribute_string_writer::write_value( size_t pos, const utils::blob& x ) {
    // "This is a binary Data string" as binary data:
    // MySQL:    X'5468697320697320612062616E617279204461746120737472696E67'
    // Postgres: E'\\x5468697320697320612062616E617279204461746120737472696E67'
    // MSSQL:    0x5468697320697320612062616E617279204461746120737472696E67
    // Sqlite:   X'5468697320697320612062616E617279204461746120737472696E67'
    result_ = conn_->dialect().token_at(dialect_token::BEGIN_BINARY_DATA) + conn_->to_escaped_string(x) + conn_->dialect().token_at(dialect_token::END_BINARY_DATA);
}

}