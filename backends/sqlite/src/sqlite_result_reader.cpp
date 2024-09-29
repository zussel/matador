#include "sqlite_result_reader.hpp"

#include <algorithm>

namespace matador::backends::sqlite {

sqlite_result_reader::sqlite_result_reader(rows result, const size_t column_count)
: result_(std::move(result))
, column_count_(column_count) {}

sqlite_result_reader::~sqlite_result_reader()
{
  std::for_each(result_.begin(), result_.end(), [](rows ::value_type& row) {
    std::for_each(row.begin(), row.end(), [](const char *val) {
      delete [] val;
    });
  });
}

size_t sqlite_result_reader::column_count() const
{
  return column_count_;
}

const char* sqlite_result_reader::column(const size_t index) const
{
  return result_[row_index_][index];
}

bool sqlite_result_reader::fetch()
{
  return ++row_index_ < result_.size();
}

}