#include "matador/utils/default_type_traits.hpp"

#include "matador/utils/value_reader.hpp"
#include "matador/utils/value_writer.hpp"

namespace matador::utils {

void data_type_traits<nullptr_t>::read_value(value_reader &/*reader*/, const char * /*id*/, size_t /*index*/, nullptr_t &/*value*/, const size_t /*size*/)
{}

void data_type_traits<nullptr_t>::bind_value(value_writer &/*binder*/, size_t /*index*/, nullptr_t &/*value*/, const size_t /*size*/)
{}

void data_type_traits<int8_t>::read_value(value_reader &reader, const char *id, const size_t index, int8_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<int8_t>::bind_value(value_writer &binder, const size_t index, const int8_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<int16_t>::read_value(value_reader &reader, const char *id, const size_t index, int16_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<int16_t>::bind_value(value_writer &binder, const size_t index, const int16_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<int32_t>::read_value(value_reader &reader, const char *id, const size_t index, int32_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<int32_t>::bind_value(value_writer &binder, const size_t index, const int32_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<int64_t>::read_value(value_reader &reader, const char *id, const size_t index, int64_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<int64_t>::bind_value(value_writer &binder, const size_t index, const int64_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<uint8_t>::read_value(value_reader &reader, const char *id, const size_t index, uint8_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<uint8_t>::bind_value(value_writer &binder, const size_t index, const uint8_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<uint16_t>::read_value(value_reader &reader, const char *id, const size_t index, uint16_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<uint16_t>::bind_value(value_writer &binder, const size_t index, const uint16_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<uint32_t>::read_value(value_reader &reader, const char *id, const size_t index, uint32_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<uint32_t>::bind_value(value_writer &binder, const size_t index, const uint32_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<uint64_t>::read_value(value_reader &reader, const char *id, const size_t index, uint64_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<uint64_t>::bind_value(value_writer &binder, const size_t index, const uint64_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<bool>::read_value(value_reader &reader, const char *id, const size_t index, bool &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<bool>::bind_value(value_writer &binder, const size_t index, const bool &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<float>::read_value(value_reader &reader, const char *id, const size_t index, float &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<float>::bind_value(value_writer &binder, const size_t index, const float &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<double>::read_value(value_reader &reader, const char *id, const size_t index, double &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<double>::bind_value(value_writer &binder, const size_t index, const double &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<const char*>::read_value(value_reader &reader, const char *id, const size_t index, const char* value, const size_t size) {
  reader.read_value(id, index, const_cast<char*>(value), size);
}

void data_type_traits<const char *>::bind_value(value_writer &binder, const size_t index, const char *value, const size_t size) {
  binder.write_value(index, value, size);
}

void data_type_traits<char*>::read_value(value_reader &reader, const char *id, const size_t index, char* value, const size_t size) {
  reader.read_value(id, index, value, size);
}

void data_type_traits<char *>::bind_value(value_writer &binder, const size_t index, const char *value, const size_t size) {
  binder.write_value(index, value, size);
}

void data_type_traits<std::string>::read_value(value_reader &reader, const char *id, const size_t index, std::string &value, const size_t size) {
  reader.read_value(id, index, value, size);
}

void data_type_traits<std::string>::bind_value(value_writer &binder, const size_t index, std::string &value, const size_t size) {
  binder.write_value(index, value, size);
}

void data_type_traits<blob_type_t>::read_value(value_reader &reader, const char *id, const size_t index, utils::blob_type_t &value, const size_t /*size*/) {
  reader.read_value(id, index, value);
}

void data_type_traits<blob_type_t>::bind_value(value_writer &binder, const size_t index, blob_type_t &value, const size_t /*size*/) {
  binder.write_value(index, value);
}

void data_type_traits<date_type_t>::read_value(value_reader &reader, const char *id, size_t index, date_type_t &value) {
  reader.read_value(id, index, value);
}

void data_type_traits<date_type_t>::bind_value(value_writer &binder, const size_t index, date_type_t &value) {
  binder.write_value(index, value);
}

void data_type_traits<time_type_t>::read_value(value_reader &reader, const char *id, size_t index, time_type_t &value) {
  reader.read_value(id, index, value);
}

void data_type_traits<time_type_t>::bind_value(value_writer &binder, const size_t index, time_type_t &value) {
  binder.write_value(index, value);
}

void data_type_traits<timestamp_type_t>::read_value(value_reader &reader, const char *id, const size_t index, timestamp_type_t &value) {
  reader.read_value(id, index, value);
}

void data_type_traits<timestamp_type_t>::bind_value(value_writer &binder, const size_t index, timestamp_type_t &value) {
  binder.write_value(index, value);
}
}
