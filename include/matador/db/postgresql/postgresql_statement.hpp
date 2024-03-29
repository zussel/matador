#ifndef MATADOR_POSTGRESQL_STATEMENT_HPP
#define MATADOR_POSTGRESQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"
#include "matador/db/postgresql/postgresql_parameter_binder.hpp"

#include <libpq-fe.h>

namespace matador {

namespace postgresql {

class postgresql_connection;

class postgresql_statement : public matador::detail::statement_impl
{
public:
  postgresql_statement(PGconn *db, detail::statement_context &&context);
  postgresql_statement(const postgresql_statement &x) = delete;
  postgresql_statement& operator=(const postgresql_statement &x) = delete;
  ~postgresql_statement() override;

  void clear() override;

  detail::result_impl *execute() override;

  void reset() override;

protected:

  detail::parameter_binder_impl *binder() const override;

private:
  static std::string generate_statement_name(const detail::statement_context &context);

private:
  PGconn *db_{nullptr};

  std::string name_;

  static std::unordered_map<std::string, unsigned long> statement_name_map_;

  PGresult *res_ = nullptr;

  std::unique_ptr<postgresql_parameter_binder> binder_;
};

}
}

#endif //MATADOR_POSTGRESQL_STATEMENT_HPP
