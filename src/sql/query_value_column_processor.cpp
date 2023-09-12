#include "matador/sql/query_value_column_processor.hpp"

namespace matador {
namespace detail {

query_value_column_processor::query_value_column_processor(
  std::shared_ptr<columns> update_columns,
  std::vector<matador::any> rowvalues
)
  : update_columns_(std::move(update_columns))
  , row_values_(std::move(rowvalues))
{
  visitor_.register_visitor<char>([this](char &val) { this->process(val); });
  visitor_.register_visitor<short>([this](short &val) { this->process(val); });
  visitor_.register_visitor<int>([this](int &val) { this->process(val); });
  visitor_.register_visitor<long>([this](long &val) { this->process(val); });
  visitor_.register_visitor<long long>([this](long long &val) { this->process(val); });
  visitor_.register_visitor<unsigned char>([this](unsigned char &val) { this->process(val); });
  visitor_.register_visitor<unsigned short>([this](unsigned short &val) { this->process(val); });
  visitor_.register_visitor<unsigned int>([this](unsigned int &val) { this->process(val); });
  visitor_.register_visitor<unsigned long>([this](unsigned long &val) { this->process(val); });
  visitor_.register_visitor<unsigned long long>([this](unsigned long long &val) { this->process(val); });
  visitor_.register_visitor<bool>([this](bool &val) { this->process(val); });
  visitor_.register_visitor<float>([this](float &val) { this->process(val); });
  visitor_.register_visitor<double>([this](double &val) { this->process(val); });
  visitor_.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  visitor_.register_visitor<char*>([this](char *val) { this->process(val); });
  visitor_.register_visitor<const char*>([this](const char *val) { this->process(val); });
  visitor_.register_visitor<matador::time>([this](matador::time &val) { this->process(val); });
  visitor_.register_visitor<matador::date>([this](matador::date &val) { this->process(val); });
}

void query_value_column_processor::execute(std::pair<std::string, matador::any> &a)
{
  current_id_ = a.first;
  row_values_.push_back(a.second);
  visitor_.visit(row_values_.back());
}

void query_value_column_processor::process(char *val)
{
  update_columns_->push_back(make_column(current_id_, val));
}

void query_value_column_processor::process(const char *val)
{
  update_columns_->push_back(make_column(current_id_, val, -1));
}

}
}