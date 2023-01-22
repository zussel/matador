#include "matador/sql/column_serializer.hpp"

namespace matador {
namespace detail {

column_serializer::column_serializer(columns::t_brackets brackets)
  : brackets_(brackets)
{}

void column_serializer::on_attribute(const char *id, char&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, short&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, int&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, long&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, long long&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned char&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned short&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned int&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned long&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, unsigned long long&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, float&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, double&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, bool&, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, char *, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, std::string &, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, date &, long /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, time &, long /*size*/)
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
