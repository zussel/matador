#include "matador/sql/statement.hpp"
#include "matador/sql/record.hpp"

namespace matador::sql {

statement::statement(std::unique_ptr<statement_impl> impl, const std::shared_ptr<basic_sql_logger> &logger)
: statement_(std::move(impl))
, object_binder_(statement_->binder())
, logger_(logger)
{}

statement &statement::bind(const size_t pos, const char *value)
{
  statement_->bind(pos, value, 0);
  return *this;
}

statement &statement::bind(const size_t pos, std::string &val, const size_t size)
{
  statement_->bind(pos, val, size);
  return *this;
}

size_t statement::execute()
{
//  logger_.info(statement_->query_.sql);
  return statement_->execute();
}

query_result<record> statement::fetch()
{
//  logger_.info(statement_->query_.sql);
  return query_result<record>{statement_->fetch()};
}

std::optional<record> statement::fetch_one()
{
//  logger_.info(statement_->query_.sql);
  query_result<record> result(statement_->fetch());
  auto first = result.begin();
  if (first == result.end()) {
    return std::nullopt;
  }

  return {*first.release()};
}

void statement::reset()
{
  statement_->reset();
}

}