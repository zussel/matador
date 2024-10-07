#include "postgres_statement.hpp"
#include "postgres_error.hpp"
#include "postgres_result_reader.hpp"

namespace matador::backends::postgres {

postgres_statement::postgres_statement(PGconn *db, PGresult *result, std::string name, const sql::query_context &query)
: statement_impl(query)
, db_(db)
, result_(result)
, name_(std::move(name))
, binder_(query_.bind_vars.size())
{}

utils::result<size_t, sql::sql_error> postgres_statement::execute()
{
  PGresult *res = PQexecPrepared(db_,
      name_.c_str(),
      static_cast<int>(binder_.params().values.size()),
      binder_.params().values.data(),
      binder_.params().lengths.data(),
      binder_.params().formats.data(),
      0);

//  throw_postgres_error(res, db_, "postgres", query_.sql);
  if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK &&
                                               status != PGRES_TUPLES_OK) {
    return utils::error(sql::sql_error::FAILURE);
  }

  const auto *tuples = PQcmdTuples(res);
  if (strlen(tuples) == 0) {
    return utils::ok(static_cast<size_t>(0));
  }
  return utils::ok(std::stoul(tuples));
}

std::unique_ptr<sql::query_result_impl> postgres_statement::fetch()
{
  PGresult *res = PQexecPrepared(db_,
      name_.c_str(),
      static_cast<int>(binder_.params().values.size()),
      binder_.params().values.data(),
      binder_.params().lengths.data(),
      binder_.params().formats.data(),
      0);

  throw_postgres_error(res, db_, "postgres", query_.sql);

  return std::make_unique<sql::query_result_impl>(std::make_unique<postgres_result_reader>(res), query_.prototype);
}

void postgres_statement::reset() {}

object::attribute_writer& postgres_statement::binder()
{
  return binder_;
}

}