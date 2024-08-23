#ifndef QUERY_MYSQL_PREPARED_RESULT_READER_HPP
#define QUERY_MYSQL_PREPARED_RESULT_READER_HPP

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
  explicit mysql_prepared_result_reader(MYSQL_STMT *stmt);

  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  bool fetch() override;

private:
  MYSQL_STMT *stmt_{};

  MYSQL_ROW current_row_{};

  size_t row_count_{};
  size_t column_count_{};
  int row_index_{-1};
};

}

#endif //QUERY_MYSQL_PREPARED_RESULT_READER_HPP
