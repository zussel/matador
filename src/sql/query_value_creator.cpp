#include "matador/sql/query_value_creator.hpp"

namespace matador {
namespace detail {

query_value_creator::query_value_creator()
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
  visitor_.register_visitor<matador::varchar_base>([this](matador::varchar_base &val) { this->process(val); });
  visitor_.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  visitor_.register_visitor<char*>([this](char *val) { this->process(val); });
  visitor_.register_visitor<const char*>([this](const char *val) { this->process(val); });
  visitor_.register_visitor<matador::time>([this](matador::time &val) { this->process(val); });
  visitor_.register_visitor<matador::date>([this](matador::date &val) { this->process(val); });
}

std::shared_ptr<matador::detail::basic_value> query_value_creator::create_from_any(any &a)
{
  visitor_.visit(a);
  return value_;
}

void query_value_creator::process(char *val)
{
  value_ = std::make_shared<value<const char*>>(val, strlen(val));
}

void query_value_creator::process(const char *val)
{
  value_ = std::make_shared<value<const char*>>(val, strlen(val));
}

}
}