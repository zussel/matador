#include "matador/sql/value_processor.hpp"
#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

namespace matador {
namespace detail {

value_processor::value_processor()
{
  visitor_.register_visitor<char>([this](char &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<int>([this](int &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<short>([this](short &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<long>([this](long &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<long long>([this](long long &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<unsigned char>([this](unsigned char &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<unsigned short>([this](unsigned short &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<unsigned int>([this](unsigned int &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<unsigned long>([this](unsigned long &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<unsigned long long>([this](unsigned long long &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<bool>([this](bool &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<float>([this](float &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<double>([this](double &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<char*>([this](char *val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<std::string>([this](std::string &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<time>([this](time &val, const field_attributes &attr) { this->process(val, attr); });
  visitor_.register_visitor<date>([this](date &val, const field_attributes &attr) { this->process(val, attr); });
}

void value_processor::apply(const char *id, matador::value &val, const field_attributes &attr, serializer &s)
{
  id_ = id;
  serializer_ = &s;
  visitor_.visit(val, attr);
  id_ = nullptr;
  serializer_ = nullptr;
}

void value_processor::process(std::string &val, const field_attributes &attr)
{
  serializer_->on_attribute(id_, val, attr);
}

void value_processor::process(char *val, const field_attributes &attr)
{
  serializer_->on_attribute(id_, val, attr);
}

void value_processor::process(time &val, const field_attributes &attr)
{
  serializer_->on_attribute(id_, val, attr);
}

void value_processor::process(date &val, const field_attributes &attr)
{
  serializer_->on_attribute(id_, val, attr);
}

value_to_string_processor::value_to_string_processor()
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

std::string value_to_string_processor::to_string(const matador::value &v)
{
  dialect_ = nullptr;
  result_.clear();
  visitor.visit(const_cast<matador::value&>(v).value_);
  return result_;
}

std::string value_to_string_processor::to_safe_string(const matador::value &v, const basic_dialect *d)
{
  dialect_ = d;
  result_.clear();
  auto a = v.value_;
  visitor.visit(const_cast<matador::value&>(v).value_);
  return result_;
}

void value_to_string_processor::process(std::string &val)
{
  if (dialect_ != nullptr) {
    result_ =  "'" + dialect_->prepare_literal(val) + "'";
  } else {
    result_ = "'" + val + "'";
  }
}

void value_to_string_processor::process(char &val)
{
  result_ = std::to_string(val);
//  std::stringstream ss;
//  ss << "'" << val << "'";
//  result_ = ss.str();
}

void value_to_string_processor::process(unsigned char &val)
{
  result_ = std::to_string(val);
}

void value_to_string_processor::process(char *val)
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

void value_to_string_processor::process(const char *val)
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

void value_to_string_processor::process(time &val)
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

void value_to_string_processor::process(date &val)
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