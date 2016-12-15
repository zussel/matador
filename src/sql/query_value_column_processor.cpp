#include "oos/sql/query_value_column_processor.hpp"

namespace oos {
namespace detail {

query_value_column_processor::query_value_column_processor(
  const std::shared_ptr<columns> &update_columns,
  const std::vector<oos::any> &rowvalues
)
  : update_columns_(update_columns)
  , rowvalues_(rowvalues)
{
  visitor_.register_visitor<char>([this](char &val) { this->process(val); });
  visitor_.register_visitor<short>([this](short &val) { this->process(val); });
  visitor_.register_visitor<int>([this](int &val) { this->process(val); });
  visitor_.register_visitor<long>([this](long &val) { this->process(val); });
  visitor_.register_visitor<unsigned char>([this](unsigned char &val) { this->process(val); });
  visitor_.register_visitor<unsigned short>([this](unsigned short &val) { this->process(val); });
  visitor_.register_visitor<unsigned int>([this](unsigned int &val) { this->process(val); });
  visitor_.register_visitor<unsigned long>([this](unsigned long &val) { this->process(val); });
  visitor_.register_visitor<bool>([this](bool &val) { this->process(val); });
  visitor_.register_visitor<float>([this](float &val) { this->process(val); });
  visitor_.register_visitor<double>([this](double &val) { this->process(val); });
  visitor_.register_visitor<oos::varchar_base>([this](oos::varchar_base &val) { this->process(val); });
  visitor_.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  visitor_.register_visitor<char*>([this](char *val) { this->process(val); });
  visitor_.register_visitor<const char*>([this](const char *val) { this->process(val); });
  visitor_.register_visitor<oos::time>([this](oos::time &val) { this->process(val); });
  visitor_.register_visitor<oos::date>([this](oos::date &val) { this->process(val); });
}

void query_value_column_processor::execute(std::pair<std::string, oos::any> &a)
{
  current_id_ = a.first;
  rowvalues_.push_back(a.second);
  visitor_.visit(rowvalues_.back());
}

void query_value_column_processor::process(char *val)
{
  std::shared_ptr<detail::value_column<const char*>> ival(new detail::value_column<const char*>(current_id_, val, strlen(val)));
  update_columns_->push_back(ival);
}

void query_value_column_processor::process(const char *val)
{
  std::shared_ptr<detail::value_column<const char*>> ival(new detail::value_column<const char*>(current_id_, val, strlen(val)));
  update_columns_->push_back(ival);
}

}
}