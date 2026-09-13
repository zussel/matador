#include "matador/query/internal/value_extractor.hpp"

namespace matador::query {
value_extractor::value_extractor(std::vector<utils::database_type> &values)
: values_(values) {
}

void value_extractor::on_revision(const char *, const uint64_t &rev) {
  data_type_traits<uint64_t>::bind_value(*this, 0, rev);
}

void value_extractor::on_attribute(const char *, char *x, const column_options &attr) {
  data_type_traits<char *>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::on_attribute(const char *, std::string &x, const column_options &attr) {
  data_type_traits<std::string>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::write_value(size_t /*pos*/, const int8_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const int16_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const int32_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const int64_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const uint8_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const uint16_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const uint32_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const uint64_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const bool &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const float &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const double &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::date_type_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::time_type_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::timestamp_type_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const char *x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const char *x, size_t /*size*/) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const std::string &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const std::string &x, size_t /*size*/) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::blob_type_t &x) {
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const column_value &/*x*/, size_t /*size*/) {
  //  values_.emplace_back(x);
}
}
