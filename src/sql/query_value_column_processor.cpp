#include "matador/sql/query_value_column_processor.hpp"

namespace matador::detail {

query_value_column_processor::query_value_column_processor(std::shared_ptr<columns> update_columns)
: update_columns_(std::move(update_columns))
{}

void query_value_column_processor::execute(std::pair<std::string, sql::column_type> &a)
{
  current_id_ = a.first;
  std::visit(*this, a.second);
}

void query_value_column_processor::process(char *val)
{
  update_columns_->push_back(make_column(current_id_, val));
}

void query_value_column_processor::process(const char *val)
{
  update_columns_->push_back(make_column(current_id_, val, -1));
}

void query_value_column_processor::process(utils::blob &val)
{
  update_columns_->push_back(make_column(current_id_, val, val.size()));
}

}
