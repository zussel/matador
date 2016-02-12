#include "sql/query_select.hpp"
#include "sql/sql.hpp"

#include "tools/identifier.hpp"
#include "tools/date.hpp"

namespace oos {

query_select::query_select(sql &s)
  : dialect(s)
  , first(true)
{}
query_select::~query_select()
{}

void query_select::serialize(const char *id, char&)
{
  serialize(id, type_char);
}

void query_select::serialize(const char *id, short&)
{
  serialize(id, type_short);
}

void query_select::serialize(const char *id, int&)
{
  serialize(id, type_int);
}

void query_select::serialize(const char *id, long&)
{
  serialize(id, type_long);
}

void query_select::serialize(const char *id, unsigned char&)
{
  serialize(id, type_unsigned_char);
}

void query_select::serialize(const char *id, unsigned short&)
{
  serialize(id, type_unsigned_short);
}

void query_select::serialize(const char *id, unsigned int&)
{
  serialize(id, type_unsigned_int);
}

void query_select::serialize(const char *id, unsigned long&)
{
  serialize(id, type_unsigned_long);
}

void query_select::serialize(const char *id, float&)
{
  serialize(id, type_float);
}

void query_select::serialize(const char *id, double&)
{
  serialize(id, type_double);
}

void query_select::serialize(const char *id, bool&)
{
  serialize(id, type_char_pointer);
}

void query_select::serialize(const char *id, char *, size_t )
{
  serialize(id, type_char_pointer);
}

void query_select::serialize(const char *id, varchar_base &)
{
  serialize(id, type_varchar);
}

void query_select::serialize(const char *id, std::string &)
{
  serialize(id, type_text);
}

void query_select::serialize(const char *id, date &)
{
  serialize(id, type_date);
}

void query_select::serialize(const char *id, time &)
{
  serialize(id, type_time);
}

void query_select::serialize(const char *id, identifiable_holder &, cascade_type)
{
  serialize(id, type_long);
}

void query_select::serialize(const char *id, data_type_t type)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(id, type);
}

}
