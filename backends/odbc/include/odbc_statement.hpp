#ifndef QUERY_ODBC_STATEMENT_HPP
#define QUERY_ODBC_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "odbc_parameter_binder.hpp"

namespace matador::backends::odbc {

class odbc_statement final : public sql::statement_impl
{
public:
  odbc_statement(sqlite3 *db, sqlite3_stmt *stmt, const sql::query_context &query);
  ~odbc_statement() override;

  size_t execute() override;
  std::unique_ptr<sql::query_result_impl> fetch() override;
  void reset() override;
protected:
  object::attribute_binder& binder() override;

private:
  sqlite3 *db_{nullptr};
  sqlite3_stmt *stmt_{nullptr};

  odbc_parameter_binder binder_;
};

}

#endif //QUERY_ODBC_STATEMENT_HPP
