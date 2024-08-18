#ifndef QUERY_ODBC_STATEMENT_HPP
#define QUERY_ODBC_STATEMENT_HPP

#include "odbc_parameter_binder.hpp"

#include "matador/sql/statement_impl.hpp"

#include <sqltypes.h>

namespace matador::backends::odbc {

class odbc_statement final : public sql::statement_impl
{
public:
  odbc_statement(SQLHANDLE stmt, const sql::query_context &query);
  ~odbc_statement() override;

  size_t execute() override;
  std::unique_ptr<sql::query_result_impl> fetch() override;
  void reset() override;
protected:
  object::attribute_binder& binder() override;

private:
  SQLHANDLE stmt_ = nullptr;

  odbc_parameter_binder binder_;
};

}

#endif //QUERY_ODBC_STATEMENT_HPP
