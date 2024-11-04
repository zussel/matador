#ifndef QUERY_POSTGRES_STATEMENT_HPP
#define QUERY_POSTGRES_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "mysql_parameter_binder.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador::backends::mysql {

class mysql_statement final : public sql::statement_impl
{
public:
  mysql_statement(MYSQL_STMT *stmt, const sql::query_context &query);
  ~mysql_statement() override;

  utils::result<size_t, sql::sql_error> execute() override;
  utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> fetch() override;
  void reset() override;

protected:
  utils::attribute_writer& binder() override;

private:
  MYSQL_STMT *stmt_{nullptr};

  std::string name_;

  mysql_parameter_binder binder_;
};

}

#endif //QUERY_POSTGRES_STATEMENT_HPP
