#ifndef MSSQL_STATEMENT_HPP
#define MSSQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"
#include "matador/sql/sql.hpp"

#include "matador/db/mssql/mssql_exception.hpp"
#include "mssql_parameter_binder.hpp"

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

namespace mssql {

class mssql_connection;

class mssql_statement : public detail::statement_impl
{
public:
  mssql_statement(SQLHANDLE db, detail::statement_context &&context);
  ~mssql_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;
  
protected:
  detail::parameter_binder_impl *binder() const override;

private:
  void create_statement();

private:
  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_ = nullptr;
  SQLHANDLE db_ = nullptr;

  std::unique_ptr<mssql_parameter_binder> binder_;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
