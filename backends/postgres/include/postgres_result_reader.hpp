#ifndef QUERY_POSTGRES_RESULT_READER_HPP
#define QUERY_POSTGRES_RESULT_READER_HPP

#include "matador/sql/query_result_reader.hpp"

#include <libpq-fe.h>

namespace matador::backends::postgres {

class postgres_result_reader final : public sql::query_result_reader
{
public:
  explicit postgres_result_reader(PGresult *result);
  ~postgres_result_reader() override;

  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  utils::result<bool, sql::sql_error> fetch() override;

  void read_value(const char *id, size_t index, utils::blob &value) override;

protected:
  utils::blob read_blob(size_t index) override;

private:
  PGresult *result_{};

  size_t row_count_{};
  size_t column_count_{};
  int row_index_{-1};
};

}

#endif //QUERY_POSTGRES_RESULT_READER_HPP
