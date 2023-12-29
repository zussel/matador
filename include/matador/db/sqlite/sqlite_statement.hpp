#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include "matador/db/sqlite/sqlite_parameter_binder.hpp"

#include "matador/sql/statement_impl.hpp"

#include <string>
#include <vector>
#include <memory>

struct sqlite3_stmt;

namespace matador {

namespace sqlite {

class sqlite_connection;

class sqlite_statement : public matador::detail::statement_impl
{
public:
  sqlite_statement(sqlite3 *db, detail::statement_context &&context);
  ~sqlite_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;

protected:

  detail::parameter_binder_impl *binder() const override;

private:
  sqlite3 *db_;
  sqlite3_stmt *stmt_;

  std::unique_ptr<sqlite_parameter_binder> binder_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
