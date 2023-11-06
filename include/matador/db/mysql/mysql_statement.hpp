#ifndef MYSQL_STATEMENT_HPP
#define MYSQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "matador/db/mysql/mysql_result_info.hpp"
#include "matador/db/mysql/mysql_parameter_binder.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

class time;
class date;

namespace mysql {

class mysql_connection;
class mysql_prepared_result;

class mysql_statement : public matador::detail::statement_impl
{
public:
  mysql_statement(MYSQL *db, detail::statement_context &&context);
  ~mysql_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;

  void unlink_result(mysql_prepared_result *result);
  
protected:
  detail::parameter_binder_impl *binder() const override;

private:
  MYSQL_STMT *stmt_ = nullptr;

  mysql_prepared_result *current_result = nullptr;

  std::unique_ptr<mysql_parameter_binder> binder_;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
