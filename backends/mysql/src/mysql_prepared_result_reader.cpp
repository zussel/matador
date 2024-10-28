#include "mysql_prepared_result_reader.hpp"

#include "matador/sql/sql_error.hpp"
#include "mysql_error.hpp"

namespace matador::backends::mysql {

mysql_prepared_result_reader::mysql_prepared_result_reader(MYSQL_STMT *stmt, MYSQL_RES *result_metadata, mysql_result_binder &&result_binder)
: stmt_(stmt)
, result_metadata_(result_metadata)
, result_binder_(std::move(result_binder))
{}

mysql_prepared_result_reader::~mysql_prepared_result_reader()
{
  if (result_metadata_) {
    mysql_free_result(result_metadata_);
  }
}

size_t mysql_prepared_result_reader::column_count() const
{
  if (result_metadata_) {
    return mysql_num_fields(result_metadata_);
  }

  return 0;
}

const char *mysql_prepared_result_reader::column(size_t index) const
{
  return nullptr;
}

bool mysql_prepared_result_reader::fetch()
{
  if (mysql_stmt_bind_result(stmt_, result_binder_.result_bindings().data()) != 0) {
    const auto err = make_error(sql::sql_error_code::BIND_FAILED, stmt_);
    return false;
//    return utils::error(make_error(sql::sql_error_code::BIND_FAILED, stmt_, query_.sql));
  }


  int ret = mysql_stmt_fetch(stmt_);
  if (ret == MYSQL_DATA_TRUNCATED) {
    // Todo: handle truncated data
//    return false;
  }
  if (ret == MYSQL_NO_DATA) {
    return false;
  }
  if (ret == 1) {
    return false;
  }

  return true;
}

void mysql_prepared_result_reader::read_value(const char *, size_t index, time &value)
{
  auto &result_binding = result_binder_.result_bindings()[index];
  auto &result_info = result_binder_.result_infos()[index];
  if (!*result_binding.is_null && result_info.length > 0) {
    auto *mtt = reinterpret_cast<MYSQL_TIME*>(result_info.buffer.data());
    value.set(static_cast<int>(mtt->year), static_cast<int>(mtt->month), static_cast<int>(mtt->day),
              static_cast<int>(mtt->hour), static_cast<int>(mtt->minute), static_cast<int>(mtt->second), mtt->second_part / 1000);
  }
  result_binding.length = nullptr;
}

void mysql_prepared_result_reader::read_value(const char *, size_t index, date &value)
{
  auto &result_binding = result_binder_.result_bindings()[index];
  auto &result_info = result_binder_.result_infos()[index];
  if (!*result_binding.is_null && result_info.length > 0) {
    auto *mtt = reinterpret_cast<MYSQL_TIME*>(result_info.buffer.data());
    value.set(static_cast<int>(mtt->day), static_cast<int>(mtt->month), static_cast<int>(mtt->year));
  }
  result_binding.length = nullptr;
}

void on_truncated_data(MYSQL_BIND &result_binding, mysql_result_info &result_info, MYSQL_STMT *stmt, size_t index, std::string &x) {
  if (result_info.length == 0) {
    return;
  }
  result_binding.buffer = new char[result_info.length];
  result_binding.buffer_length = result_info.length;
  if (mysql_stmt_fetch_column(stmt, &result_binding, index, 0) == 0) {
    auto *data = (char*)result_binding.buffer;
    unsigned long len = result_binding.buffer_length;
    x.assign(data, len);
  } else {
    // Todo: handle statement fetch column error
  }
  delete [] (char*)result_binding.buffer;
  result_binding.buffer = nullptr;
  result_binding.length = nullptr;
}

void on_truncated_data(MYSQL_BIND &result_binding, mysql_result_info &result_info, MYSQL_STMT *stmt, size_t index, utils::blob &x) {
  if (result_info.length == 0) {
    return;
  }
  result_binding.buffer = new char[result_info.length];
  result_binding.buffer_length = result_info.length;
  if (mysql_stmt_fetch_column(stmt, &result_binding, index, 0) == 0) {
    auto *data = reinterpret_cast<unsigned char*>(result_binding.buffer);
    unsigned long len = result_binding.buffer_length;
    x.assign(data, data+len);
  } else {
    // Todo: handle statement fetch column error
  }
  delete [] (char*)result_binding.buffer;
  result_binding.buffer = nullptr;
  result_binding.length = nullptr;
}

void mysql_prepared_result_reader::read_value(const char *, size_t index, std::string &value, size_t size)
{
  auto &result_binding = result_binder_.result_bindings()[index];
  auto &result_info = result_binder_.result_infos()[index];
  if (*result_binding.error) {
    // assume truncated data
    on_truncated_data(result_binding, result_info, stmt_, index, value);
  } else if (*result_binding.is_null == false) {
    auto *data = (char *) result_binding.buffer;
    unsigned long len = result_info.length;
    value.assign(data, len);
  } else {
    value.clear();
  }
}

void mysql_prepared_result_reader::read_value(const char *, size_t index, utils::blob &value)
{
  auto &result_binding = result_binder_.result_bindings()[index];
  auto &result_info = result_binder_.result_infos()[index];
  if (*result_binding.error) {
    // assume truncated data
    on_truncated_data(result_binding, result_info, stmt_, index, value);
  } else if (*result_binding.is_null == false) {
    auto *data = (char *) result_binding.buffer;
    unsigned long len = result_info.length;
    value.assign(data, data + len);
  } else {
    value.clear();
  }
}

object::attribute_reader &mysql_prepared_result_reader::result_binder()
{
  return result_binder_;
}

}