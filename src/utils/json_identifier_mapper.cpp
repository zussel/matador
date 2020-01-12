//
// Created by sascha on 12.01.20.
//

#include "matador/utils/json_identifier_mapper.hpp"
#include "matador/utils/basic_identifier.hpp"

#include <cstring>

namespace matador {
void json_identifier_mapper::set_identifier_value(basic_identifier &id, const value_t *val)
{
  value_ = val;
  id.serialize("", *this);
  value_ = nullptr;
}

void json_identifier_mapper::serialize(const char *, char &x)
{
  x = (char)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, short &x)
{
  x = (short)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, int &x)
{
  x = (int)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, long &x)
{
  x = (long long)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, long long &x)
{
  x = value_->integer;
}

void json_identifier_mapper::serialize(const char *, unsigned char &x)
{
  x = (unsigned char)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, unsigned short &x)
{
  x = (unsigned short)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, unsigned int &x)
{
  x = (unsigned int)(value_->integer);
}


void json_identifier_mapper::serialize(const char *, unsigned long &x)
{
  x = (unsigned long)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, unsigned long long &x)
{
  x = (unsigned long long)(value_->integer);
}

void json_identifier_mapper::serialize(const char *, bool &x)
{
  x = value_->boolean;
}

void json_identifier_mapper::serialize(const char *, float &x)
{
  x = (float)(value_->real);
}

void json_identifier_mapper::serialize(const char *, double &x)
{
  x = value_->real;
}

void json_identifier_mapper::serialize(const char *, char *x, size_t s)
{
  strncpy(x, value_->str.c_str(), s);
}

void json_identifier_mapper::serialize(const char *, std::string &x)
{
  x = value_->str;
}

void json_identifier_mapper::serialize(const char *, std::string &x, size_t)
{
  x = value_->str;
}
}