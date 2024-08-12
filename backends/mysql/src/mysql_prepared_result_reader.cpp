#include "mysql_prepared_result_reader.hpp"

namespace matador::backends::mysql {

mysql_prepared_result_reader::mysql_prepared_result_reader(MYSQL_STMT *stmt)
: stmt_(stmt)
{}

mysql_prepared_result_reader::~mysql_prepared_result_reader()
{

}

size_t mysql_prepared_result_reader::column_count() const
{
  return 0;
}

const char *mysql_prepared_result_reader::column(size_t index) const
{
  return nullptr;
}

bool mysql_prepared_result_reader::fetch()
{
  return false;
}
}