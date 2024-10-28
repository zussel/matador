#ifndef QUERY_MYSQL_PREPARED_RESULT_READER_HPP
#define QUERY_MYSQL_PREPARED_RESULT_READER_HPP

#include "mysql_result_binder.hpp"

#include "matador/sql/query_result_reader.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador::backends::mysql {

class mysql_prepared_result_reader : public sql::query_result_reader
{
public:
  mysql_prepared_result_reader(MYSQL_STMT *stmt, MYSQL_RES *result_metadata, mysql_result_binder &&result_binder);
  ~mysql_prepared_result_reader() override;

  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  bool fetch() override;

  void read_value(const char * /*id*/, size_t /*index*/, char &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, short &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, int &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, long &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, long long &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, unsigned char &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, unsigned short &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, unsigned int &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, unsigned long &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, unsigned long long &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, bool &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, float &/*value*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, double &/*value*/) override {}
  void read_value(const char * /*id*/, size_t index, matador::time &value) override;
  void read_value(const char * /*id*/, size_t index, matador::date &value) override;
  void read_value(const char * /*id*/, size_t /*index*/, char * /*value*/, size_t /*size*/) override {}
  void read_value(const char * /*id*/, size_t /*index*/, std::string &/*value*/) override {}
  void read_value(const char * /*id*/, size_t index, std::string &value, size_t size) override;
  void read_value(const char * /*id*/, size_t index, utils::blob &value) override;

protected:
  object::attribute_reader &result_binder() override;

private:
  MYSQL_STMT *stmt_{};
  MYSQL_RES *result_metadata_{};

  MYSQL_ROW current_row_{};

  mysql_result_binder result_binder_;

  size_t row_count_{};
  size_t column_count_{};
  int row_index_{-1};
};

}

#endif //QUERY_MYSQL_PREPARED_RESULT_READER_HPP
