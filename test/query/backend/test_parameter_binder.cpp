#include "test_parameter_binder.hpp"

namespace matador::test::orm {
void test_parameter_binder::write_value(size_t /*pos*/, const int8_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const int16_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const int32_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const int64_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const uint8_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const uint16_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const uint32_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const uint64_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const bool &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const float &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const double &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const utils::date_type_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const utils::time_type_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const utils::timestamp_type_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const char * /*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const char * /*x*/, size_t /*size*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const std::string &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const std::string &/*x*/, size_t /*size*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const utils::blob_type_t &/*x*/) {}
void test_parameter_binder::write_value(size_t /*pos*/, const query::column_value &/*x*/, size_t /*size*/) {}
}
