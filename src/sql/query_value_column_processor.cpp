#include "matador/sql/query_value_column_processor.hpp"

namespace matador {
namespace detail {

query_value_column_processor::query_value_column_processor()
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

std::unique_ptr<columns> query_value_column_processor::execute(const std::vector<std::pair<std::string, matador::any>> &column_values)
{
  update_columns_ = std::make_unique<columns>();

  for (const auto &column_value : column_values) {
    current_id_ = column_value.first;
    row_values_.push_back(column_value.second);
    visitor_.visit(row_values_.back());
  }

  return std::move(update_columns_);
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