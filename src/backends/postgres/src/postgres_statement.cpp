#include "postgres_statement.hpp"

#include "postgres_error.hpp"
#include "postgres_parameter_binder.h"
#include "postgres_result_reader.hpp"

#include "matador/query/runtime/execute_result.hpp"

#include "matador/query/internal/query_result_impl.hpp"

namespace matador::backends::postgres {

postgres_statement::postgres_statement(PGconn *db, PGresult *res, std::string name, const query::query_context &query)
: statement_impl(query, 0)
, db_(db)
, res_(res)
, name_(std::move(name))
{}

postgres_statement::~postgres_statement() {
  PQclear(res_);
}

result<query::execute_result, error> postgres_statement::execute(const query::value_writer& bindings) {
  const auto* postgres_bindings = dynamic_cast<const postgres_parameter_binder*>(&bindings);
  if (!postgres_bindings) {
    return failure(error(query::error_code::ExecuteFailed, "Failed to cast bindings to postgres bindings"));
  }

  if (query_.command == query::query_command::Insert) {
    // handle insert command with the primary key generator strategy
  }

  PGresult *res = PQexecPrepared(db_,
      name_.c_str(),
      static_cast<int>(postgres_bindings->params().values.size()),
      postgres_bindings->params().values.data(),
      postgres_bindings->params().lengths.data(),
      postgres_bindings->params().formats.data(),
      0);

  if (is_result_error(res)) {
    return failure(make_error(query::error_code::ExecuteFailed, res, db_, "Failed to execute statement", query_.sql));
  }

  size_t value{0};
  if (const auto *tuples = PQcmdTuples(res); strlen(tuples) != 0) {
    value = std::stoul(tuples);
  }

  PQclear(res);

  return ok(query::execute_result{value});
}

result<std::unique_ptr<query::query_result_impl>, error> postgres_statement::fetch(const query::value_writer& bindings) {
  const auto* postgres_bindings = dynamic_cast<const postgres_parameter_binder*>(&bindings);
  if (!postgres_bindings) {
    return failure(error(query::error_code::ExecuteFailed, "Failed to cast bindings to postgres bindings"));
  }
  PGresult *res = PQexecPrepared(db_,
      name_.c_str(),
      static_cast<int>(postgres_bindings->params().values.size()),
      postgres_bindings->params().values.data(),
      postgres_bindings->params().lengths.data(),
      postgres_bindings->params().formats.data(),
      0);

  if (is_result_error(res)) {
    return failure(make_error(query::error_code::FetchFailed, res, db_, "Failed to fetch statement", query_.sql));
  }

  return ok(std::make_unique<query::query_result_impl>(std::make_unique<postgres_result_reader>(res),
                                                                    query_.columns,
                                                                    query_.resolver,
                                                                    query_.result_type));
}

std::unique_ptr<query::value_writer> postgres_statement::create_binder() const {
  return std::make_unique<postgres_parameter_binder>(query_.bind_vars.size());
}
}
