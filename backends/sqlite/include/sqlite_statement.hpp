#ifndef QUERY_SQLITE_STATEMENT_HPP
#define QUERY_SQLITE_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "sqlite_parameter_binder.h"

namespace matador::backends::sqlite {

class sqlite_statement final : public sql::statement_impl
{
public:
  sqlite_statement(sqlite3 *db, sqlite3_stmt *stmt, const sql::query_context &query);
  ~sqlite_statement() override;

  size_t execute() override;
  std::unique_ptr<sql::query_result_impl> fetch() override;
  void reset() override;
protected:
  object::attribute_binder& binder() override;

private:
  sqlite3 *db_{nullptr};
  sqlite3_stmt *stmt_{nullptr};

  sqlite_parameter_binder binder_;
};

}

#endif //QUERY_SQLITE_STATEMENT_HPP
