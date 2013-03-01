#include "query_create.hpp"

#include "database.hpp"

#include <cstring>

query_create::query_create(sql &d, const database &db)
  : dialect(d)
  , db_(db)
  , first(true)
{}

query_create::~query_create()
{}

void query_create::write(const char *id, long )
{
  write(id, sql::type_long);
}
void query_create::write(const char *id, double )
{
  write(id, sql::type_double);
}
void query_create::write(const char *id, const std::string &)
{
  write(id, sql::type_text);
}

void query_create::write(const char *id, sql::data_type_t type)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(std::string(id) + " ");
  dialect.append(db_.type_string(type));
  if (strcmp(id, "id") == 0) {
    dialect.append(" NOT NULL PRIMARY KEY");
  }
}
