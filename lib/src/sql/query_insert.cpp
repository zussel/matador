#include "sql/query_insert.hpp"

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

void query_insert::serialize(const char *id, char &x)
{
  write_field(id, type_char, x);
}

void query_insert::serialize(const char *id, short &x)
{
  write_field(id, type_short, x);
}

void query_insert::serialize(const char *id, int &x)
{
  write_field(id, type_int, x);
}

void query_insert::serialize(const char *id, long &x)
{
  write_field(id, type_long, x);
}

void query_insert::serialize(const char *id, unsigned char &x)
{
  write_field(id, type_unsigned_char, x);
}

void query_insert::serialize(const char *id, unsigned short &x)
{
  write_field(id, type_unsigned_short, x);
}

void query_insert::serialize(const char *id, unsigned int &x)
{
  write_field(id, type_unsigned_int, x);
}

void query_insert::serialize(const char *id, unsigned long &x)
{
  write_field(id, type_unsigned_long, x);
}

void query_insert::serialize(const char *id, float &x)
{
  write_field(id, type_float, x);
}

void query_insert::serialize(const char *id, double &x)
{
  write_field(id, type_double, x);
}

void query_insert::serialize(const char *id, bool &x)
{
  write_field(id, type_char_pointer, x);
}

void query_insert::serialize(const char *id, char *x, size_t len)
{
  write_field(id, type_char_pointer, x, len);
}

void query_insert::serialize(const char *id, varchar_base &x)
{
  write_field(id, type_varchar, x);
}

void query_insert::serialize(const char *id, std::string &x)
{
  write_field(id, type_text, x);
}

void query_insert::serialize(const char *id, date &x)
{
  write_field(id, type_date, x);
}

void query_insert::serialize(const char *id, time &x)
{
  write_field(id, type_time, x);
}

void query_insert::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

void query_insert::serialize(const char *id, basic_object_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    write_null(id);
  }
}

void query_insert::write_field(const char *id, data_type_t type, char &x) {
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

void query_insert::write_field(const char *id, data_type_t type, oos::date &x)
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

void query_insert::write_field(const char *id, data_type_t type, oos::time &x)
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

void query_insert::write_field(const char *id, data_type_t type, std::string &x)
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

void query_insert::write_field(const char *id, data_type_t type, varchar_base &x)
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

void query_insert::write_field(const char *id, data_type_t type, char *x, size_t)
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

void query_insert::write_null(const char *id)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  if (fields_) {
    dialect.append(id);
  } else {
    dialect.append(id, type_long, "NULL");
  }
}

}
