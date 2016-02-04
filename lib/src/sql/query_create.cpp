#include "sql/query_create.hpp"
#include "sql/connection.hpp"
#include "sql/sql.hpp"

#include "tools/date.hpp"

#include <sstream>
#include <cstring>

namespace oos {

query_create::query_create(sql &d, const connection &db)
  : dialect(d)
  , db_(db)
  , first(true)
{}

query_create::~query_create()
{}

void query_create::serialize(const char *id, char&)
{
  serialize(id, type_char);
}

void query_create::serialize(const char *id, short&)
{
  serialize(id, type_short);
}

void query_create::serialize(const char *id, int&)
{
  serialize(id, type_int);
}

void query_create::serialize(const char *id, long&)
{
  serialize(id, type_long);
}

void query_create::serialize(const char *id, unsigned char&)
{
  serialize(id, type_unsigned_char);
}

void query_create::serialize(const char *id, unsigned short&)
{
  serialize(id, type_unsigned_short);
}

void query_create::serialize(const char *id, unsigned int&)
{
  serialize(id, type_unsigned_int);
}

void query_create::serialize(const char *id, unsigned long&)
{
  serialize(id, type_unsigned_long);
}

void query_create::serialize(const char *id, float&)
{
  serialize(id, type_float);
}

void query_create::serialize(const char *id, double&)
{
  serialize(id, type_double);
}

void query_create::serialize(const char *id, bool&)
{
  serialize(id, type_bool);
}

void query_create::serialize(const char *id, char *, size_t s)
{
  serialize(id, type_char_pointer, s);
}

void query_create::serialize(const char *id, varchar_base &x)
{
  serialize(id, type_varchar, x.capacity());
}

void query_create::serialize(const char *id, std::string &)
{
  serialize(id, type_text);
}

void query_create::serialize(const char *id, date &)
{
  serialize(id, type_date);
}

void query_create::serialize(const char *id, time &)
{
  serialize(id, type_time);
}

void query_create::serialize(const char *id, basic_object_holder &)
{
  serialize(id, type_long);
}

void query_create::serialize(const char *id, data_type_t type)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + " ");
  // TODO: fix call to type_string
  dialect.append(db_.type_string(type));
}

void query_create::serialize(const char *id, data_type_t type, size_t size)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + " ");

  std::stringstream t;
  t << db_.type_string(type) << "(" << size << ")";
  dialect.append(t.str());
}

void query_create::append(const std::string &term)
{
  dialect.append(term);
}

}
