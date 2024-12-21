#ifndef QUERY_ODBC_STATEMENT_HPP
#define QUERY_ODBC_STATEMENT_HPP

#include "odbc_bound_value.hpp"
#include "odbc_parameter_binder.hpp"

#include "matador/sql/statement_impl.hpp"

#include <sqltypes.h>

namespace matador::backends::odbc {

class odbc_statement final : public sql::statement_impl
{
public:
  odbc_statement(SQLHANDLE stmt, std::vector<odbc_bound_value> &&bound_values, const sql::query_context &query);
  ~odbc_statement() override;

  utils::result<size_t, sql::sql_error> execute() override;
  utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> fetch() override;
  void reset() override;

protected:
  utils::attribute_writer& binder() override;
  size_t start_index() const override;
  size_t adjust_index( size_t index ) const override;

private:
    void cleanup() const;

private:
  SQLHANDLE stmt_ = nullptr;

  odbc_parameter_binder binder_;
};

}

#endif //QUERY_ODBC_STATEMENT_HPP
