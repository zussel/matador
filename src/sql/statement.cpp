#include "matador/sql/statement.hpp"
#include "matador/sql/record.hpp"

namespace matador::sql {

statement::statement(std::unique_ptr<statement_impl> impl, const std::shared_ptr<basic_sql_logger> &logger)
: statement_(std::move(impl))
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

utils::result<size_t, sql_error> statement::execute() const
{
//  logger_.info(statement_->query_.sql);
  return statement_->execute();
}

utils::result<query_result<record>, sql_error> statement::fetch() const
{
  auto result = statement_->fetch();
  if (!result.is_ok()) {
    return utils::error(result.err());
  }
//  logger_.info(statement_->query_.sql);
  return utils::ok(query_result<record>{std::move(*result)});
}

utils::result<std::optional<record>, sql_error> statement::fetch_one() const {
    //  logger_.info(statement_->query_.sql);
    auto result = statement_->fetch();
    if (!result.is_ok()) {
        return utils::error(result.err());
    }

    query_result<record> records(std::move(*result));
    auto first = records.begin();
    if (first == records.end()) {
        return utils::ok(std::optional<record>{std::nullopt});
    }

    return utils::ok(std::optional{*first.release()});
}

void statement::reset() const
{
  statement_->reset();
}

}