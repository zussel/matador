#include "matador/sql/column_serializer.hpp"

namespace matador {
namespace detail {

column_serializer::column_serializer(columns::t_brackets brackets)
  : brackets_(brackets)
{}

void column_serializer::on_attribute(const char *id, char&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, short&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, int&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, long&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, long long&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned char&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned short&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned int&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned long&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned long long&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, float&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, double&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, bool&, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, char *, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, std::string &, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, date &, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, time &, const field_attributes &/*attr*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_belongs_to(const char *id, identifiable_holder &, cascade_type)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_has_one(const char *id, identifiable_holder &, cascade_type)
{
  cols_->push_back(std::make_shared<column>(id));
}

}
}
