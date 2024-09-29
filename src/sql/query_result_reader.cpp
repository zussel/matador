#include "matador/sql/query_result_reader.hpp"
#include "matador/sql/to_value.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/string.hpp"

namespace matador::sql {
size_t query_result_reader::start_column_index() const
{
  return 0;
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, char &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, short &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, int &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, long &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, long long int &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, unsigned char &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, unsigned short &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, unsigned int &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, unsigned long &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, unsigned long long int &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, bool &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, float &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, double &value)
{
  to_value(value, column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, time &value)
{
  if (const auto val = column(index); strlen(val) > 0) {
    value = time::parse(val, "%Y-%m-%d %T.%f");
  }
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, date &value)
{
  if (const auto val = column(index); strlen(val) > 0) {
    value.set(val, matador::utils::date_format::ISO8601);
  }
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, char *value, size_t size)
{
  auto val = column(index);
  if (const size_t len = strlen(val); len > size) {
#ifdef _MSC_VER
    strncpy_s(value, size, val, len);
#else
    strncpy(value, val, size);
#endif
    value[size-1] = '\n';
  } else {
#ifdef _MSC_VER
    strcpy_s(value, size, val);
#else
    strcpy(value, val);
#endif
  }
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, std::string &value)
{
  value.assign(column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, std::string &value, size_t /*s*/)
{
  value.assign(column(index));
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, utils::blob &value)
{
  const auto val = column(index);
  const auto len = strlen(val);
  value.assign(val, val+len);
}

template < typename Type >
void convert(const char *val_str, value &val)
{
  Type local_val{};
  to_value(local_val, val_str);
  val = local_val;
}

void query_result_reader::read_value(const char * /*id*/, const size_t index, value &val, size_t /*size*/)
{
  switch (val.type()) {
    case data_type::type_char:
      convert<char>(column(index), val);
      break;
    case data_type::type_short:
      convert<short>(column(index), val);
      break;
    case data_type::type_int:
      convert<int>(column(index), val);
      break;
    case data_type::type_long:
      convert<long>(column(index), val);
      break;
    case data_type::type_long_long:
      convert<long long>(column(index), val);
      break;
    case data_type::type_unsigned_char:
      convert<unsigned char>(column(index), val);
      break;
    case data_type::type_unsigned_short:
      convert<unsigned short>(column(index), val);
      break;
    case data_type::type_unsigned_int:
      convert<unsigned int>(column(index), val);
      break;
    case data_type::type_unsigned_long:
      convert<unsigned long>(column(index), val);
      break;
    case data_type::type_unsigned_long_long:
      convert<unsigned long long>(column(index), val);
      break;
    case data_type::type_float:
      convert<float>(column(index), val);
      break;
    case data_type::type_double:
      convert<double>(column(index), val);
      break;
    case data_type::type_bool: {
      int local_val{};
      to_value(local_val, column(index));
      val = local_val > 0;
      break;
    }
    case data_type::type_text:
    case data_type::type_varchar: {
      val = std::string{column(index)};
      break;
    }
    case data_type::type_char_pointer: {
      val = column(index);
      break;
    }
    case data_type::type_time:
    case data_type::type_date: {
      val = std::string{column(index)};
      break;
    }
    case data_type::type_null: {
      val = nullptr_t{};
      break;
    }
    case data_type::type_blob: {
      const auto *data = column(index);
      const auto len = strlen(data);
      val = utils::blob{data, data+len};
      break;
    }
    case data_type::type_unknown: {
      val = std::string(column(index));
      break;
    }
  }
}

}