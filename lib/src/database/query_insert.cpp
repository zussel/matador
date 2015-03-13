#include "database/query_insert.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <sstream>
#include <object/primary_key.hpp>
#include <tools/date.hpp>

namespace oos {

query_insert::query_insert(sql &s)
  : dialect(s)
  , first(true)
  , fields_(true)
{}
query_insert::~query_insert() {}

void query_insert::write(const char *id, char x)
{
  write_field(id, type_char, x);
}

void query_insert::write(const char *id, short x)
{
  write_field(id, type_short, x);
}

void query_insert::write(const char *id, int x)
{
  write_field(id, type_int, x);
}

void query_insert::write(const char *id, long x)
{
  write_field(id, type_long, x);
}

void query_insert::write(const char *id, unsigned char x)
{
  write_field(id, type_unsigned_char, x);
}

void query_insert::write(const char *id, unsigned short x)
{
  write_field(id, type_unsigned_short, x);
}

void query_insert::write(const char *id, unsigned int x)
{
  write_field(id, type_unsigned_int, x);
}

void query_insert::write(const char *id, unsigned long x)
{
  write_field(id, type_unsigned_long, x);
}

void query_insert::write(const char *id, float x)
{
  write_field(id, type_float, x);
}

void query_insert::write(const char *id, double x)
{
  write_field(id, type_double, x);
}

void query_insert::write(const char *id, bool x)
{
  write_field(id, type_char_pointer, x);
}

void query_insert::write(const char *id, const char *x, int)
{
  write_field(id, type_char_pointer, x);
}

void query_insert::write(const char *id, const varchar_base &x)
{
  write_field(id, type_varchar, x);
}

void query_insert::write(const char *id, const std::string &x)
{
  write_field(id, type_text, x);
}

void query_insert::write(const char *id, const date &x)
{
  // TODO: implement write date
}

void query_insert::write(const char *id, const time &x)
{
  // TODO: implement write time
}

void query_insert::write(const char *id, const object_base_ptr &x)
{
  write_field(id, type_long, x.id());
}

void query_insert::write(const char *, const object_container &)
{}

void query_insert::write(const char *id, const primary_key_base &x)
{
  x.serialize(id, *this);
}

void query_insert::write_field(const char *id, data_type_t type, const std::string &x)
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

void query_insert::write_field(const char *id, data_type_t type, const varchar_base &x)
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
    valstr << "'" << x.str() << "'";
    dialect.append(id, type, valstr.str());
  }
}

void query_insert::write_field(const char *id, data_type_t type, const char *x)
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

}
