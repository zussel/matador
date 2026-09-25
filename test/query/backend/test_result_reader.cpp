#include "test_result_reader.hpp"

#include "matador/query/column_value.hpp"

namespace matador::test::orm {

size_t test_result_reader::column_count() const {
  return 10;
}

const char *test_result_reader::column(size_t /*index*/) const {
  return "";
}

utils::result<bool, utils::error> test_result_reader::fetch() { return utils::ok(rows_-- > 0); }

size_t test_result_reader::start_column_index() const {
  return 0;
}

void test_result_reader::unshift() {}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, int8_t &value) {
  value = -8;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, int16_t &value) {
  value = -16;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, int32_t &value) {
  value = -32;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, int64_t &value) {
  value = -64;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, uint8_t &value) {
  value = 8;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, uint16_t &value) {
  value = 16;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, uint32_t &value) {
  value = 32;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, uint64_t &value) {
  value = 64;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, bool &value) {
  value = true;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, float &value) {
  value = 3.141572f;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, double &value) {
  value = 2.14159265358979323846;
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, utils::date_type_t &value) {
  value = {2021, 10, 27};
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, utils::time_type_t &value) {
  value = {12, 34, 56, 123456};
}

void test_result_reader::read_value(const char * /*id*/, size_t /*index*/, utils::timestamp_type_t &value) {
  value = utils::timestamp_type_t();
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, char * /*value*/, const size_t /*size*/) {
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, std::string &value) {
  value = "Lorem ipsum";
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, std::string &value, const size_t /*size*/) {
  value = "Hello world";
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, utils::blob_type_t &value) {
  value = {'b', 'l', 'o', 'b'};
}

void test_result_reader::read_value(const char * /*id*/, const size_t /*index*/, query::column_value &val, const size_t /*size*/) {
  val = "value";
}
query::value_reader &test_result_reader::result_binder() {
  return empty_binder_;
}

} // namespace matador::test::orm
