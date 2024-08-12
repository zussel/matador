#include "postgres_result_reader.hpp"

#include "matador/sql/to_value.hpp"

namespace matador::backends::postgres {

postgres_result_reader::postgres_result_reader(PGresult *result)
: result_(result)
, row_count_(PQntuples(result_))
, column_count_(PQnfields(result_))
{}

postgres_result_reader::~postgres_result_reader()
{
  if (result_) {
    PQclear(result_);
  }
}

size_t postgres_result_reader::column_count() const
{
  return column_count_;
}

const char *postgres_result_reader::column(size_t index) const
{
  return PQgetvalue(result_, static_cast<int>(row_index_), static_cast<int>(index));
}

bool postgres_result_reader::fetch()
{
  return ++row_index_ < row_count_;
}

}