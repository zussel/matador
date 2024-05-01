#include "matador/sql/value_processor.hpp"
#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

namespace matador::detail {

value_processor::value_processor(serializer &serializer)
: serializer_(serializer)
{}

void value_processor::apply(const char *id, matador::value &val, const field_attributes &attr)
{
  id_ = id;
  attributes_ = attr;
  std::visit(*this, const_cast<sql::column_type &>(val()));
  id_ = nullptr;
}

void value_processor::process(std::string &val, const field_attributes &attr)
{
  serializer_.on_attribute(id_, val, attr);
}

void value_processor::process(const char *val, const field_attributes &attr)
{
  serializer_.on_attribute(id_, const_cast<char*>(val), attr);
}

void value_processor::process(time &val, const field_attributes &attr)
{
  serializer_.on_attribute(id_, val, attr);
}

void value_processor::process(date &val, const field_attributes &attr)
{
  serializer_.on_attribute(id_, val, attr);
}

void value_processor::process(utils::blob &/*val*/, const field_attributes &/*attr*/) {
//  serializer_->on_attribute(id_, val, attr);
}

value_to_string_processor::value_to_string_processor(const basic_dialect &dialect)
: dialect_(dialect)
{}

std::string value_to_string_processor::to_string(const matador::value &val)
{
  result_.clear();
  style_ = style::Compact;
  std::visit(*this, const_cast<sql::column_type &>(val()));
  return result_;
}

std::string value_to_string_processor::to_safe_string(const matador::value &val)
{
  result_.clear();
  style_ = style::Safe;
  std::visit(*this, const_cast<sql::column_type &>(val()));
  return result_;
}

void value_to_string_processor::process(std::string &val)
{
  if (style_ == style::Safe) {
    result_ =  "'" + dialect_.prepare_literal(val) + "'";
  } else {
    result_ = "'" + val + "'";
  }
}

void value_to_string_processor::process(char &val) {
  result_ = std::to_string(val);
}

void value_to_string_processor::process(unsigned char &val)
{
  result_ = std::to_string(val);
}

void value_to_string_processor::process(char *val)
{
  if (style_ == style::Safe) {
    std::stringstream ss;
    ss << "'" << dialect_.prepare_literal(val) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << val << "'";
    result_ = ss.str();
  }
}

void value_to_string_processor::process(const char *val)
{
  if (style_ == style::Safe) {
    std::stringstream ss;
    ss << "'" << dialect_.prepare_literal(val) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << val << "'";
    result_ = ss.str();
  }
}

void value_to_string_processor::process(time &val)
{
  if (style_ == style::Safe) {
    std::stringstream ss;
    ss << "'" << dialect_.prepare_literal(matador::to_string(val, "%FT%T.%f")) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << matador::to_string(val, "%FT%T.%f") << "'";
    result_ = ss.str();
  }
}

void value_to_string_processor::process(date &val)
{
  if (style_ == style::Safe) {
    std::stringstream ss;
    ss << "'" << dialect_.prepare_literal(matador::to_string(val)) << "'";
    result_ = ss.str();
  } else {
    std::stringstream ss;
    ss << "'" << matador::to_string(val) << "'";
    result_ = ss.str();
  }
}

void value_to_string_processor::process(utils::blob &/*val*/) {

}
}
