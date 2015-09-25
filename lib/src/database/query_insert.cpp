#include "database/query_insert.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"
#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/string.hpp"

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
  write_field(id, type_date, x);
}

void query_insert::write(const char *id, const time &x)
{
  write_field(id, type_time, x);
}

void query_insert::write(const char *id, const object_base_ptr &x)
{
  write_field(id, type_long, x.id());
}

void query_insert::write(const char *, const object_container &)
{}

void query_insert::write(const char *id, const basic_identifier &x)
{
  x.serialize(id, *this);
}


void query_insert::write_field(const char *id, data_type_t type, const char &x) {
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

void query_insert::write_field(const char *id, data_type_t type, const oos::date &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  // TODO: append query insert part for date
  if (fields_) {
    dialect.append(id);
  } else {
    std::stringstream valstr;
    valstr << "'" << to_string(x) << "'";
    dialect.append(id, type, valstr.str());
  }
}

void query_insert::write_field(const char *id, data_type_t type, const oos::time &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  // TODO: append query insert part for date
  if (fields_) {
    dialect.append(id);
  } else {
    std::stringstream valstr;
    valstr << "'" << to_string(x, "%F %T.%f") << "'";
    dialect.append(id, type, valstr.str());
  }
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
