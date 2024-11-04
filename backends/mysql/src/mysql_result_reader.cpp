#include "mysql_result_reader.hpp"

namespace matador::backends::mysql {

mysql_result_reader::mysql_result_reader(MYSQL_RES *result, unsigned int column_count)
: result_(result)
, row_count_(mysql_num_rows(result_))
, column_count_(column_count)
{}

mysql_result_reader::~mysql_result_reader()
{
  if (result_) {
    mysql_free_result(result_);
  }
}

size_t mysql_result_reader::column_count() const
{
  return column_count_;
}

const char *mysql_result_reader::column(size_t index) const
{
  return current_row_[index];
}

utils::result<bool, sql::sql_error> mysql_result_reader::fetch()
{
  current_row_ = mysql_fetch_row(result_);

  return utils::ok(current_row_ != nullptr);
}

}