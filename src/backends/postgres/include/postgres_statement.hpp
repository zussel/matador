#ifndef QUERY_POSTGRES_STATEMENT_HPP
#define QUERY_POSTGRES_STATEMENT_HPP

#include "matador/query/interface/statement_impl.hpp"
#include "matador/query/mapping/value_writer.hpp"

#include <libpq-fe.h>

namespace matador::backends::postgres {

class postgres_statement final : public query::statement_impl
{
public:
  postgres_statement(PGconn *db, PGresult *res, std::string name, const query::query_context &query);
  ~postgres_statement() override;

  result<query::execute_result, error> execute(const query::value_writer& bindings) override;
  result<std::unique_ptr<query::query_result_impl>, error> fetch(const query::value_writer& bindings) override;

protected:
  [[nodiscard]] std::unique_ptr<query::value_writer> create_binder() const override;

private:
  PGconn *db_{nullptr};
  PGresult *res_ = nullptr;

  std::string name_;
};

}

#endif //QUERY_POSTGRES_STATEMENT_HPP
