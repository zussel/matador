//
// Created by sascha on 03.06.19.
//

#ifndef MATADOR_POSTGRESQL_STATEMENT_HPP
#define MATADOR_POSTGRESQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"
#include "matador/db/postgresql/postgresql_parameter_binder.hpp"

#include <libpq-fe.h>

namespace matador {

class varchar_base;

class basic_identifier;

namespace postgresql {

class postgresql_connection;

class postgresql_statement : public matador::detail::statement_impl
{
public:
  postgresql_statement(postgresql_connection &db, const matador::sql &stmt);
  postgresql_statement(const postgresql_statement &x) = delete;
  postgresql_statement& operator=(const postgresql_statement &x) = delete;
  ~postgresql_statement() override;

  void clear() override;

  detail::result_impl *execute() override;

  void reset() override;

protected:

  detail::parameter_binder_impl *binder() const override;

private:
  static std::string generate_statement_name(const matador::sql &stmt);

private:
  postgresql_connection &db_;

  std::string name_;

  static std::unordered_map<std::string, unsigned long> statement_name_map_;

  PGresult *res_ = nullptr;

  std::unique_ptr<postgresql_parameter_binder> binder_;
};

}
}

#endif //MATADOR_POSTGRESQL_STATEMENT_HPP
