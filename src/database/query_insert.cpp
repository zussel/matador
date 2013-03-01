#include "query_insert.hpp"

query_insert::query_insert(sql &s)
  : dialect(s)
  , first(true)
  , fields_(true)
{}
query_insert::~query_insert() {}

void query_insert::write(const char *id, long x)
{
  write_field(id, sql::type_long, x);
}
void query_insert::write(const char *id, double x)
{
  write_field(id, sql::type_double, x);
}
void query_insert::write(const char *id, const std::string &x)
{
  write_field(id, sql::type_text, x);
}

void query_insert::write_field(const char *id, sql::data_type_t type, const std::string &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  if (fields_) {
    dialect.append(id);
  } else {
    std::stringstream valstr;
    valstr << "'" << x << "'";
    dialect.append(id, type, valstr.str());
  }
}

void query_insert::fields()
{
  fields_ = true;
  first = true;
}

void query_insert::values()
{
  fields_ = false;
  first = true;
}
