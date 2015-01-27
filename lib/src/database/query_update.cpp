#include <object/primary_key.hpp>
#include "database/query_update.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

namespace oos {

query_update::query_update(sql &s)
  : dialect(s)
  , first(true)
{}

query_update::~query_update() {}

void query_update::write(const char *id, char x)
{
  write_pair(id, type_char, x);
}

void query_update::write(const char *id, short x)
{
  write_pair(id, type_short, x);
}

void query_update::write(const char *id, int x)
{
  write_pair(id, type_int, x);
}

void query_update::write(const char *id, long x)
{
  write_pair(id, type_long, x);
}

void query_update::write(const char *id, unsigned char x)
{
  write_pair(id, type_unsigned_char, x);
}

void query_update::write(const char *id, unsigned short x)
{
  write_pair(id, type_unsigned_short, x);
}

void query_update::write(const char *id, unsigned int x)
{
  write_pair(id, type_unsigned_int, x);
}

void query_update::write(const char *id, unsigned long x)
{
  write_pair(id, type_unsigned_long, x);
}

void query_update::write(const char *id, float x)
{
  write_pair(id, type_float, x);
}

void query_update::write(const char *id, double x)
{
  write_pair(id, type_double, x);
}

void query_update::write(const char *id, bool x)
{
  write_pair(id, type_char_pointer, x);
}

void query_update::write(const char *id, const char *x, int)
{
  write_pair(id, type_char_pointer, x);
}

void query_update::write(const char *id, const varchar_base &x)
{
  write_pair(id, type_varchar, x);
}

void query_update::write(const char *id, const std::string &x)
{
  write_pair(id, type_text, x);
}

void query_update::write(const char *id, const object_base_ptr &x)
{
  write_pair(id, type_long, x.id());
}

void query_update::write(const char *, const object_container &)
{}

void query_update::write(const char *id, const primary_key_base &x)
{
  x.serialize(id, *this);
}

void query_update::write_pair(const char *id, data_type_t type, const std::string &x)
{
    if (first) {
      first = false;
    } else {
      dialect.append(", ");
    }
    dialect.append(std::string(id) + "=");
    std::stringstream valstr;
    valstr << "'" << x << "'";
    dialect.append(id, type, valstr.str());
}

void query_update::write_pair(const char *id, data_type_t type, const varchar_base &x)
{
    if (first) {
      first = false;
    } else {
      dialect.append(", ");
    }
    dialect.append(std::string(id) + "=");
    std::stringstream valstr;
    valstr << "'" << x.str() << "'";
    dialect.append(id, type, valstr.str());
}

void query_update::write_pair(const char *id, data_type_t type, const char *x)
{
    if (first) {
      first = false;
    } else {
      dialect.append(", ");
    }
    dialect.append(std::string(id) + "=");
    std::stringstream valstr;
    valstr << "'" << x << "'";
    dialect.append(id, type, valstr.str());
}

}
