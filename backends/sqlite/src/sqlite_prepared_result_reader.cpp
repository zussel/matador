#include "sqlite_prepared_result_reader.hpp"
#include "sqlite_error.hpp"

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
  if (ret != SQLITE_ROW) {
    throw_sqlite_error(ret, db_, "sqlite3_step");
  }
  return ret != SQLITE_DONE;
}

void sqlite_prepared_result_reader::read_value(const char * /*id*/, size_t index, char &value)
{
  value = static_cast<char>(sqlite3_column_int(stmt_, static_cast<int>(index)));
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, short &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, int &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, long &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, long long int &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, unsigned char &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, unsigned short &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, unsigned int &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, unsigned long &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, unsigned long long int &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, bool &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, float &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, double &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, char *value, size_t s)
{
  query_result_reader::read_value(id, index, value, s);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, std::string &value)
{
  query_result_reader::read_value(id, index, value);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, std::string &value, size_t s)
{
  query_result_reader::read_value(id, index, value, s);
}

void sqlite_prepared_result_reader::read_value(const char *id, size_t index, sql::value &val, size_t size)
{
  query_result_reader::read_value(id, index, val, size);
}
}