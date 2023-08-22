#include "matador/sql/value_processor.hpp"
#include "matador/sql/basic_dialect.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

namespace matador {
namespace detail {

value_processor::value_processor()
{
  visitor.register_visitor<char>([this](char &val) { this->process(val); });
  visitor.register_visitor<int>([this](int &val) { this->process(val); });
  visitor.register_visitor<short>([this](short &val) { this->process(val); });
  visitor.register_visitor<long>([this](long &val) { this->process(val); });
  visitor.register_visitor<long long>([this](long long &val) { this->process(val); });
  visitor.register_visitor<unsigned char>([this](unsigned char &val) { this->process(val); });
  visitor.register_visitor<unsigned short>([this](unsigned short &val) { this->process(val); });
  visitor.register_visitor<unsigned int>([this](unsigned int &val) { this->process(val); });
  visitor.register_visitor<unsigned long>([this](unsigned long &val) { this->process(val); });
  visitor.register_visitor<unsigned long long>([this](unsigned long long &val) { this->process(val); });
  visitor.register_visitor<bool>([this](bool &val) { this->process(val); });
  visitor.register_visitor<float>([this](float &val) { this->process(val); });
  visitor.register_visitor<double>([this](double &val) { this->process(val); });
  visitor.register_visitor<char*>([this](char *val) { this->process(val); });
  visitor.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  visitor.register_visitor<time>([this](time &val) { this->process(val); });
  visitor.register_visitor<date>([this](date &val) { this->process(val); });
}

void value_processor::apply(matador::any &a, const char *id, serializer &s)
{
  id_ = id;
  serializer_ = &s;
  visitor.visit(a);
  id_ = nullptr;
  serializer_ = nullptr;
}

void value_processor::process(std::string &val)
{
  serializer_->on_attribute(id_, val, {});
}

void value_processor::process(char *val)
{
  serializer_->on_attribute(id_, val, {});
}

void value_processor::process(time &val)
{
  serializer_->on_attribute(id_, val, {});
}

void value_processor::process(date &val)
{
  serializer_->on_attribute(id_, val, {});
}

value_to_string_visitor::value_to_string_visitor()
{
  visitor.register_visitor<char>([this](char &val) { this->process(val); });
  visitor.register_visitor<int>([this](int &val) { this->process(val); });
  visitor.register_visitor<short>([this](short &val) { this->process(val); });
  visitor.register_visitor<long>([this](long &val) { this->process(val); });
  visitor.register_visitor<long long>([this](long long &val) { this->process(val); });
  visitor.register_visitor<unsigned char>([this](unsigned char &val) { this->process(val); });
  visitor.register_visitor<unsigned int>([this](unsigned int &val) { this->process(val); });
  visitor.register_visitor<unsigned short>([this](unsigned short &val) { this->process(val); });
  visitor.register_visitor<unsigned long>([this](unsigned long &val) { this->process(val); });
  visitor.register_visitor<unsigned long long>([this](unsigned long long &val) { this->process(val); });
  visitor.register_visitor<bool>([this](bool &val) { this->process(val); });
  visitor.register_visitor<float>([this](float &val) { this->process(val); });
  visitor.register_visitor<double>([this](double &val) { this->process(val); });
  visitor.register_visitor<char*>([this](char *val) { this->process(val); });
  visitor.register_visitor<const char*>([this](const char *val) { this->process(val); });
  visitor.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  visitor.register_visitor<time>([this](time &val) { this->process(val); });
  visitor.register_visitor<date>([this](date &val) { this->process(val); });
}

std::string value_to_string_visitor::to_string(matador::any &a)
{
  dialect_ = nullptr;
  result_.clear();
  visitor.visit(a);
  return result_;
}

std::string value_to_string_visitor::to_safe_string(matador::any &a, const basic_dialect *d)
{
  dialect_ = d;
  result_.clear();
  visitor.visit(a);
  return result_;
}

void value_to_string_visitor::process(std::string &val)
{
  if (dialect_ != nullptr) {
    result_ =  "'" + dialect_->prepare_literal(val) + "'";
  } else {
    result_ = "'" + val + "'";
  }
}

void value_to_string_visitor::process(char &val)
{
  std::stringstream ss;
  ss << "'" << val << "'";
  result_ = ss.str();
}

void value_to_string_visitor::process(unsigned char &val)
{
  std::stringstream ss;
  ss << "'" << val << "'";
  result_ = ss.str();
}

void value_to_string_visitor::process(char *val)
{
  if (dialect_ != nullptr) {
    std::stringstream ss;
    ss << "'" << dialect_->prepare_literal(val) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << val << "'";
    result_ = ss.str();
  }
}

void value_to_string_visitor::process(const char *val)
{
  if (dialect_ != nullptr) {
    std::stringstream ss;
    ss << "'" << dialect_->prepare_literal(val) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << val << "'";
    result_ = ss.str();
  }
}

void value_to_string_visitor::process(time &val)
{
  if (dialect_ != nullptr) {
    std::stringstream ss;
    ss << "'" << dialect_->prepare_literal(matador::to_string(val, "%FT%T.%f")) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << matador::to_string(val, "%FT%T.%f") << "'";
    result_ = ss.str();
  }
}

void value_to_string_visitor::process(date &val)
{
  if (dialect_ != nullptr) {
    std::stringstream ss;
    ss << "'" << dialect_->prepare_literal(matador::to_string(val)) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << matador::to_string(val) << "'";
    result_ = ss.str();
  }
}

}
}