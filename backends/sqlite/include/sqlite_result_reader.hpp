#ifndef QUERY_SQLITE_RESULT_READER_HPP
#define QUERY_SQLITE_RESULT_READER_HPP

#include "matador/sql/query_result_reader.hpp"

#include <vector>

namespace matador::backends::sqlite {

class sqlite_result_reader final : public sql::query_result_reader
{
public:
  using columns = std::vector<char*>;
  using rows = std::vector<columns>;

public:
  sqlite_result_reader(rows result, size_t column_count);
  ~sqlite_result_reader() override;

  [[nodiscard]] size_t column_count() const override;

  [[nodiscard]] const char* column(size_t index) const override;
  [[nodiscard]] utils::result<bool, sql::sql_error> fetch() override;

private:
  rows result_;
  long long row_index_ = -1;
  size_t column_count_{};
};

}

#endif //QUERY_SQLITE_RESULT_READER_HPP
