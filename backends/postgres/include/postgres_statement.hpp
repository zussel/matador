#ifndef QUERY_POSTGRES_STATEMENT_HPP
#define QUERY_POSTGRES_STATEMENT_HPP

#include "matador/sql/sql_error.hpp"
#include "matador/sql/statement_impl.hpp"

#include "postgres_parameter_binder.h"

#include <libpq-fe.h>

namespace matador::backends::postgres {

class postgres_statement final : public sql::statement_impl
{
public:
  postgres_statement(PGconn *db, PGresult *result, std::string name, const sql::query_context &query);

  utils::result<size_t, sql::sql_error> execute() override;
  std::unique_ptr<sql::query_result_impl> fetch() override;
  void reset() override;
protected:
  object::attribute_writer& binder() override;

private:
  PGconn *db_{nullptr};
  PGresult *result_{nullptr};

  std::string name_;

  postgres_parameter_binder binder_;
};

}

#endif //QUERY_POSTGRES_STATEMENT_HPP
