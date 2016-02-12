#include "sql/query_update.hpp"

#include "tools/identifier.hpp"
#include "object/object_ptr.hpp"

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"

namespace oos {

query_update::query_update(sql &s)
  : dialect(s)
  , first(true)
{}

query_update::~query_update() {}

void query_update::serialize(const char *id, char &x)
{
  write_pair(id, type_char, x);
}

void query_update::serialize(const char *id, short &x)
{
  write_pair(id, type_short, x);
}

void query_update::serialize(const char *id, int &x)
{
  write_pair(id, type_int, x);
}

void query_update::serialize(const char *id, long &x)
{
  write_pair(id, type_long, x);
}

void query_update::serialize(const char *id, unsigned char &x)
{
  write_pair(id, type_unsigned_char, x);
}

void query_update::serialize(const char *id, unsigned short &x)
{
  write_pair(id, type_unsigned_short, x);
}

void query_update::serialize(const char *id, unsigned int &x)
{
  write_pair(id, type_unsigned_int, x);
}

void query_update::serialize(const char *id, unsigned long &x)
{
  write_pair(id, type_unsigned_long, x);
}

void query_update::serialize(const char *id, float &x)
{
  write_pair(id, type_float, x);
}

void query_update::serialize(const char *id, double &x)
{
  write_pair(id, type_double, x);
}

void query_update::serialize(const char *id, bool &x)
{
  write_pair(id, type_char_pointer, x);
}

void query_update::serialize(const char *id, char *x, size_t)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + "=");
  std::stringstream valstr;
  valstr << "'" << x << "'";
  dialect.append(id, type_char_pointer, valstr.str());
}

void query_update::serialize(const char *id, varchar_base &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + "=");
  std::stringstream valstr;
  valstr << "'" << x.str() << "'";
  dialect.append(id, type_varchar, valstr.str());
}

void query_update::serialize(const char *id, std::string &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + "=");
  std::stringstream valstr;
  valstr << "'" << x << "'";
  dialect.append(id, type_text, valstr.str());
}

void query_update::serialize(const char *id, date &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + "=");
  // TODO: append query update part for time
  std::stringstream valstr;
  valstr << x;
  dialect.append(id, type_date, valstr.str());
}

void query_update::serialize(const char *id, time &x)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + "=");
  // TODO: append query update part for time
  std::stringstream valstr;
  valstr << x;
  dialect.append(id, type_time, valstr.str());
}

void query_update::serialize(const char *id, identifiable_holder &x)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  }
}

}
