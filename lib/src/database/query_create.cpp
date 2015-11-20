#include "database/query_create.hpp"
#include "database/database.hpp"

#include "object/basic_identifier.hpp"

#include "tools/varchar.hpp"
#include "tools/date.hpp"

#include <sstream>
#include <cstring>

namespace oos {

query_create::query_create(sql &d, const database &db)
  : dialect(d)
  , db_(db)
  , first(true)
{}

query_create::~query_create()
{}

void query_create::write(const char *id, char)
{
  write(id, type_char);
}

void query_create::write(const char *id, short)
{
  write(id, type_short);
}

void query_create::write(const char *id, int)
{
  write(id, type_int);
}

void query_create::write(const char *id, long)
{
  write(id, type_long);
}

void query_create::write(const char *id, unsigned char)
{
  write(id, type_unsigned_char);
}

void query_create::write(const char *id, unsigned short)
{
  write(id, type_unsigned_short);
}

void query_create::write(const char *id, unsigned int)
{
  write(id, type_unsigned_int);
}

void query_create::write(const char *id, unsigned long)
{
  write(id, type_unsigned_long);
}

void query_create::write(const char *id, float)
{
  write(id, type_float);
}

void query_create::write(const char *id, double)
{
  write(id, type_double);
}

void query_create::write(const char *id, bool)
{
  write(id, type_bool);
}

void query_create::write(const char *id, const char *, size_t s)
{
  write(id, type_char_pointer, s);
}

void query_create::write(const char *id, const varchar_base &x)
{
  write(id, type_varchar, x.capacity());
}

void query_create::write(const char *id, const std::string &)
{
  write(id, type_text);
}

void query_create::write(const char *id, const date &)
{
  write(id, type_date);
}

void query_create::write(const char *id, const time &)
{
  write(id, type_time);
}

void query_create::write(const char *id, const object_base_ptr &)
{
  write(id, type_long);
}

void query_create::write(const char *, const object_container &)
{}

void query_create::write(const char *id, const basic_identifier &x)
{
  x.serialize(id, *this);
//  dialect.append(" NOT NULL PRIMARY KEY AUTOINCREMENT");
  dialect.append(" NOT NULL PRIMARY KEY");
}

void query_create::write(const char *id, data_type_t type)
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

void query_create::write(const char *id, data_type_t type, size_t size)
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

}
