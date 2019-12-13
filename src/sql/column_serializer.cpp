//
// Created by sascha on 04.03.16.
//

#include "matador/sql/column_serializer.hpp"

namespace matador {
namespace detail {

column_serializer::column_serializer(columns::t_brackets brackets)
  : brackets_(brackets)
{}

void column_serializer::serialize(const char *id, char&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, short&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, int&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, long&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, long long&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, unsigned char&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, unsigned short&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, unsigned int&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, unsigned long&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, unsigned long long&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, float&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, double&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, bool&)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, char *, size_t)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, std::string &, size_t)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, std::string &)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, date &)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, time &)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, identifiable_holder &, cascade_type)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::serialize(const char *id, basic_identifier &)
{
  cols_->push_back(std::make_shared<column>(id));
}

}
}
