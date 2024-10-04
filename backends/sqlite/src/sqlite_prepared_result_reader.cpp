#include "sqlite_prepared_result_reader.hpp"
#include "sqlite_error.hpp"

#include "matador/utils/string.hpp"

#include <cstring>

namespace matador::backends::sqlite {

sqlite_prepared_result_reader::sqlite_prepared_result_reader(sqlite3 *db, sqlite3_stmt *stmt)
: db_(db)
, stmt_(stmt)
{}

size_t sqlite_prepared_result_reader::column_count() const
{
  return sqlite3_column_count(stmt_);
}

const char *sqlite_prepared_result_reader::column(size_t index) const
{
  return reinterpret_cast<const char*>(sqlite3_column_text(stmt_, static_cast<int>(index)));
}

bool sqlite_prepared_result_reader::fetch()
{
  int ret = sqlite3_step(stmt_);
  if (ret != SQLITE_ROW && ret != SQLITE_DONE) {
    throw_sqlite_error(ret, db_, "sqlite3_step");
  }
  return ret == SQLITE_ROW;
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, char &value)
{
  value = static_cast<char>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, short &value)
{
  value = static_cast<short>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, int &value)
{
  value = static_cast<int>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, long &value)
{
  value = static_cast<long>(sqlite3_column_int64(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, long long int &value)
{
  value = static_cast<long long>(sqlite3_column_int64(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, unsigned char &value)
{
  value = static_cast<unsigned char>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, unsigned short &value)
{
  value = static_cast<unsigned short>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, unsigned int &value)
{
  value = static_cast<unsigned int>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, unsigned long &value)
{
  value = static_cast<unsigned long>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, unsigned long long int &value)
{
  value = static_cast<unsigned long long>(sqlite3_column_int64(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, bool &value)
{
  value = sqlite3_column_int(stmt_, static_cast<int>(index)) > 0;
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, float &value)
{
  value = static_cast<float>(sqlite3_column_double(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, double &value)
{
  value = sqlite3_column_double(stmt_, static_cast<int>(index));
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, char *value, size_t s)
{
  auto size = static_cast<size_t>(sqlite3_column_bytes(stmt_, static_cast<int>(index)));
  if (size >= s) {
    --size;
  }
#ifdef _MSC_VER
  strncpy_s(value, s, reinterpret_cast<const char*>(sqlite3_column_text(stmt_, static_cast<int>(index))), size);
#else
  strncpy(value, reinterpret_cast<const char*>(sqlite3_column_text(stmt_, static_cast<int>(index))), size);
#endif
  value[size] = '\0';
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, std::string &value)
{
  const auto s = static_cast<size_t>(sqlite3_column_bytes(stmt_, static_cast<int>(index)));
  const auto *text = sqlite3_column_text(stmt_, static_cast<int>(index));
  if (s > 0) {
    value.assign(reinterpret_cast<const char*>(text), s);
  }
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, std::string &value, size_t /*size*/)
{
  read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, sql::value &val, size_t size)
{
  query_result_reader::read_value(id, index, val, size);
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, time &value) {
  const auto is_null = sqlite3_column_type(stmt_, static_cast<int>(index)) == SQLITE_NULL;
  if (const auto s = static_cast<size_t>(sqlite3_column_bytes(stmt_, static_cast<int>(index))); !is_null && s > 0) {
    const auto *text = reinterpret_cast<const char *>( sqlite3_column_text(stmt_, static_cast<int>(index)));
    value = time::parse(text, "%Y-%m-%d %T.%f");
  }
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, date &value) {
  const auto is_null = sqlite3_column_type(stmt_, static_cast<int>(index)) == SQLITE_NULL;
  if (const auto s = static_cast<size_t>(sqlite3_column_bytes(stmt_, static_cast<int>(index))); !is_null && s > 0) {
    const auto *text = reinterpret_cast<const char *>( sqlite3_column_text(stmt_, static_cast<int>(index)));
    value = date::parse(text, matador::utils::date_format::ISO8601);
  }
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, utils::blob &value) {
  auto is_null = sqlite3_column_type(stmt_, static_cast<int>(index)) == SQLITE_NULL;
  if (const auto s = static_cast<size_t>(sqlite3_column_bytes(stmt_, static_cast<int>(index))); !is_null && s > 0) {
    const auto *data = static_cast<const unsigned char*>( sqlite3_column_blob(stmt_, static_cast<int>(index)));
    value.assign(data, data+s);
  }
}
}