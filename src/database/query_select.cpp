#include "query_select.hpp"

query_select::query_select(sql &s)
  : dialect(s)
  , first(true)
{}
query_select::~query_select()
{}

void query_select::write(const char *id, long )
{
  write(id, sql::type_long);
}
void query_select::write(const char *id, double )
{
  write(id, sql::type_double);
}
void query_select::write(const char *id, const std::string &)
{
  write(id, sql::type_text);
}

void query_select::write(const char *id, sql::data_type_t type)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(id, type);
}
