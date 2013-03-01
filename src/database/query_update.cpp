#include "query_update.hpp"

query_update::query_update(sql &s)
  : dialect(s)
  , first(true)
{}

query_update::~query_update() {}

void query_update::write(const char *id, long x)
{
  write_pair(id, sql::type_long, x);
}

void query_update::write(const char *id, double x)
{
  write_pair(id, sql::type_double, x);
}

void query_update::write(const char *id, const std::string &x)
{
  write_pair(id, sql::type_text, x);
}

void query_update::write_pair(const char *id, sql::data_type_t type, const std::string &x)
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
