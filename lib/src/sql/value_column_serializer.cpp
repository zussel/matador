//
// Created by sascha on 03.04.16.
//

#include "sql/value_column_serializer.hpp"

namespace oos {
namespace detail {

value_column_serializer::value_column_serializer(sql &d)
  : sql_(d)
{ }

void value_column_serializer::serialize(const char *id, char &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, short &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, int &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, long &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned char &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned short &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned int &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned long &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, float &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, double &x)
{
  cols_->push_back(std::make_shared<value_column>(id, x));
}

void value_column_serializer::serialize(const char *id, bool &x)
{

}

void value_column_serializer::serialize(const char *id, char *x, size_t s)
{

}

void value_column_serializer::serialize(const char *id, varchar_base &x)
{

}

void value_column_serializer::serialize(const char *id, std::string &x)
{

}

void value_column_serializer::serialize(const char *id, date &x)
{

}

void value_column_serializer::serialize(const char *id, time &x)
{

}

void value_column_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{

}

void value_column_serializer::serialize(const char *id, basic_identifier &x)
{

}

}
}
