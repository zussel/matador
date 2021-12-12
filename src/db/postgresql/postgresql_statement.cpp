#include "matador/db/postgresql/postgresql_exception.hpp"
#include "matador/db/postgresql/postgresql_statement.hpp"
#include "matador/db/postgresql/postgresql_prepared_result.hpp"
#include "matador/db/postgresql/postgresql_connection.hpp"

#include "matador/utils/identifier.hpp"

#include "matador/sql/sql.hpp"

namespace matador {

namespace postgresql {

std::unordered_map<std::string, unsigned long> postgresql_statement::statement_name_map_ = std::unordered_map<std::string, unsigned long>();

postgresql_statement::postgresql_statement(postgresql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , db_(db)
{
  binder_ = std::make_unique<postgresql_parameter_binder>(bind_vars().size());
  // parse sql to create result and host arrays
  name_ = generate_statement_name(stmt);

  res_ = PQprepare(db.handle(), name_.c_str(), str().c_str(), bind_vars().size(), nullptr);

  throw_database_error(res_, db_.handle(), "postgresql", str());
}

postgresql_statement::~postgresql_statement()
{
  clear();
}

void postgresql_statement::clear()
{
  if (res_ != nullptr) {
    PQclear(res_);
    res_ = nullptr;
  }
}

detail::result_impl *postgresql_statement::execute()
{
  PGresult *res = PQexecPrepared(db_.handle(), name_.c_str(), binder_->params().size(), binder_->params().data(), nullptr, nullptr, 0);

  throw_database_error(res, db_.handle(), "postgresql", str());

  return new postgresql_prepared_result(this, res);
}

void postgresql_statement::reset()
{
}

std::string postgresql_statement::generate_statement_name(const matador::sql &stmt)
{
  std::stringstream name;
  name << stmt.table_name() << "_" << stmt.command();
  auto result = postgresql_statement::statement_name_map_.find(name.str());

  if (result == postgresql_statement::statement_name_map_.end()) {
    result = postgresql_statement::statement_name_map_.insert(std::make_pair(name.str(), 0)).first;
  }

  name << "_" << ++result->second;

  return name.str();
}

detail::parameter_binder_impl *postgresql_statement::binder() const
{
  return binder_.get();
}

}
}