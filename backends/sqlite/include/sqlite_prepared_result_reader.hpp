#ifndef QUERY_SQLITE_PREPARED_RESULT_READER_HPP
#define QUERY_SQLITE_PREPARED_RESULT_READER_HPP

#include "matador/sql/query_result_reader.hpp"

#include <sqlite3.h>

namespace matador::backends::sqlite {

class sqlite_prepared_result_reader final : public sql::query_result_reader
{
public:
  sqlite_prepared_result_reader(sqlite3 *db, sqlite3_stmt *stmt);

  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  bool fetch() override;

  void read_value(const char *id, size_t index, char &value) override;
  void read_value(const char *id, size_t index, short &value) override;
  void read_value(const char *id, size_t index, int &value) override;
  void read_value(const char *id, size_t index, long &value) override;
  void read_value(const char *id, size_t index, long long int &value) override;
  void read_value(const char *id, size_t index, unsigned char &value) override;
  void read_value(const char *id, size_t index, unsigned short &value) override;
  void read_value(const char *id, size_t index, unsigned int &value) override;
  void read_value(const char *id, size_t index, unsigned long &value) override;
  void read_value(const char *id, size_t index, unsigned long long int &value) override;
  void read_value(const char *id, size_t index, bool &value) override;
  void read_value(const char *id, size_t index, float &value) override;
  void read_value(const char *id, size_t index, double &value) override;
  void read_value(const char *id, size_t index, char *value, size_t s) override;
  void read_value(const char *id, size_t index, std::string &value) override;
  void read_value(const char *id, size_t index, std::string &value, size_t s) override;
  void read_value(const char *id, size_t index, sql::value &val, size_t size) override;
  void read_value(const char *id, size_t index, time &value) override;
  void read_value(const char *id, size_t index, date &value) override;
  void read_value(const char *id, size_t index, utils::blob &value) override;

private:
  sqlite3 *db_{nullptr};
  sqlite3_stmt *stmt_{nullptr};
};
}
#endif //QUERY_SQLITE_PREPARED_RESULT_READER_HPP
