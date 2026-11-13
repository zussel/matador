#include "postgres_result_reader.hpp"

#include "matador/utils/convert.hpp"
#include "matador/query/model/column_value.hpp"
#include "matador/utils/string.hpp"

namespace matador::backends::postgres {
postgres_result_reader::postgres_result_reader(PGresult *result)
: result_(result)
  , row_count_(PQntuples(result_))
  , column_count_(PQnfields(result_)) {
}

postgres_result_reader::~postgres_result_reader() {
  if (result_) {
    PQclear(result_);
  }
}

size_t postgres_result_reader::column_count() const {
  return column_count_;
}

const char *postgres_result_reader::column(const size_t index) const {
  return PQgetvalue(result_, static_cast<int>(row_index_), static_cast<int>(index));
}

result<bool, error> postgres_result_reader::fetch() { return ok(++row_index_ < row_count_); }

size_t postgres_result_reader::start_column_index() const {
  return 0;
}

void postgres_result_reader::unshift() {
  if (row_index_ > 0) {
    --row_index_;
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, int8_t &value) {
  if (auto res = utils::to<int8_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, int16_t &value) {
  if (auto res = utils::to<int16_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, int32_t &value) {
  if (auto res = utils::to<int32_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, int64_t &value) {
  if (auto res = utils::to<int64_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, uint8_t &value) {
  if (auto res = utils::to<uint8_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, uint16_t &value) {
  if (auto res = utils::to<uint16_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, uint32_t &value) {
  if (auto res = utils::to<uint32_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, uint64_t &value) {
  if (auto res = utils::to<uint64_t>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, bool &value) {
  if (auto res = utils::to<bool>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, float &value) {
  if (auto res = utils::to<float>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, double &value) {
  if (auto res = utils::to<double>(column(index)); res.is_ok()) {
    value = res.value();
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, utils::date_type_t &value) {
  if (const auto val = column(index); strlen(val) > 0) {
    if (const auto res = utils::to<utils::date_type_t>(std::string(val)); res.is_ok()) {
      value = res.value();
    }
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, utils::time_type_t &value) {
  if (const auto val = column(index); strlen(val) > 0) {
    if (const auto res = utils::to<utils::time_type_t>(std::string(val)); res.is_ok()) {
      value = res.value();
    }
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, utils::timestamp_type_t &value) {
  if (const auto val = column(index); strlen(val) > 0) {
    if (const auto res = utils::to<utils::timestamp_type_t>(std::string(val)); res.is_ok()) {
      value = res.value();
    }
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, char *value, size_t size) {
  auto val = column(index);
  if (const size_t len = strlen(val); len > size) {
#ifdef _MSC_VER
    strncpy_s(value, size, val, len);
#else
    strncpy(value, val, size);
#endif
    value[size - 1] = '\n';
  } else {
#ifdef _MSC_VER
    strcpy_s(value, size, val);
#else
    strcpy(value, val);
#endif
  }
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, std::string &value) {
  value.assign(column(index));
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, std::string &value, size_t /*s*/) {
  value.assign(column(index));
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, utils::blob_type_t &value) {
  const auto *data = reinterpret_cast<const unsigned char *>(column(index));

  size_t length;
  unsigned char *unescaped = PQunescapeBytea(data, &length);

  value.assign(unescaped, unescaped + length);

  PQfreemem(unescaped);
}

template<typename Type>
void set_value(const char *str, query::column_value &value) {
  if (const auto res = utils::to<Type>(str); res.is_ok()) {
    value = res.value();
  }
}

template<>
void set_value<utils::blob_type_t>(const char *str, query::column_value &value) {
  size_t length;
  unsigned char *unescaped = PQunescapeBytea(reinterpret_cast<const unsigned char *>(str), &length);

  value = utils::blob_type_t(unescaped, unescaped + length);

  PQfreemem(unescaped);
}

void postgres_result_reader::read_value(const char * /*id*/, const size_t index, query::column_value &val, size_t) {
  switch (val.type()) {
    case utils::basic_type::Int8:
      set_value<int8_t>(column(index), val);
      break;
    case utils::basic_type::Int16:
      set_value<int16_t>(column(index), val);
      break;
    case utils::basic_type::Int32:
      set_value<int32_t>(column(index), val);
      break;
    case utils::basic_type::Int64:
      set_value<int64_t>(column(index), val);
      break;
    case utils::basic_type::UInt8:
      set_value<uint8_t>(column(index), val);
      break;
    case utils::basic_type::UInt16:
      set_value<uint16_t>(column(index), val);
      break;
    case utils::basic_type::UInt32:
      set_value<uint32_t>(column(index), val);
      break;
    case utils::basic_type::UInt64:
      set_value<uint64_t>(column(index), val);
      break;
    case utils::basic_type::Float:
      set_value<float>(column(index), val);
      break;
    case utils::basic_type::Double:
      set_value<double>(column(index), val);
      break;
    case utils::basic_type::Boolean:
      set_value<bool>(column(index), val);
      break;
    case utils::basic_type::Text:
    case utils::basic_type::Varchar: {
      if (const auto *column_value = column(index); column_value == nullptr) {
        val = std::string{};
      } else {
        val = std::string{column_value};
      }
      break;
    }
    case utils::basic_type::Time:
      set_value<utils::time_type_t>(column(index), val);
      break;
    case utils::basic_type::Date:
      set_value<utils::date_type_t>(column(index), val);
      break;
    case utils::basic_type::DateTime: {
      set_value<utils::timestamp_type_t>(column(index), val);
      break;
    }
    case utils::basic_type::Null: {
      val = nullptr_t{};
      break;
    }
    case utils::basic_type::Blob: {
      set_value<utils::blob_type_t>(column(index), val);
      break;
    }
    case utils::basic_type::Unknown:
      break;
  }
}

query::value_reader &postgres_result_reader::result_binder() {
  return empty_binder_;
}
} // namespace matador::backends::postgres
