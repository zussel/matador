//
// Created by sascha on 03.06.19.
//

#include "matador/db/postgresql/postgresql_statement.hpp"
#include "matador/db/postgresql/postgresql_connection.hpp"

#include "matador/sql/sql.hpp"

namespace matador {

namespace postgresql {

std::unordered_map<std::string, unsigned long> postgresql_statement::statement_name_map_ = {};

postgresql_statement::postgresql_statement(postgresql_connection &db, const matador::sql &stmt)
  : db_(db)
{
  // generate sql
  str(db.dialect()->prepare(stmt));
  // parse sql to create result and host arrays
  result_size = db.dialect()->column_count();
  host_size = db.dialect()->bind_count();

  name_ = generate_statement_name(stmt);

  PQprepare(db.handle(), name_.c_str(), str().c_str(), host_size, nullptr);
}

postgresql_statement::~postgresql_statement()
{
  clear();
}

void postgresql_statement::clear()
{

}

detail::result_impl *postgresql_statement::execute()
{
  PGresult *res = PQexecPrepared(db_.handle(), name_.c_str(), host_size, nullptr, nullptr, nullptr, 0);
  return nullptr;
}

void postgresql_statement::reset()
{

}

void postgresql_statement::serialize(const char *id, char &x)
{

}

void postgresql_statement::serialize(const char *id, short &x)
{

}

void postgresql_statement::serialize(const char *id, int &x)
{

}

void postgresql_statement::serialize(const char *id, long &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned char &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned short &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned int &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned long &x)
{

}

void postgresql_statement::serialize(const char *id, float &x)
{

}

void postgresql_statement::serialize(const char *id, double &x)
{

}

void postgresql_statement::serialize(const char *id, bool &x)
{

}

void postgresql_statement::serialize(const char *id, char *x, size_t s)
{

}

void postgresql_statement::serialize(const char *id, varchar_base &x)
{

}

void postgresql_statement::serialize(const char *id, std::string &x)
{

}

void postgresql_statement::serialize(const char *id, matador::date &x)
{

}

void postgresql_statement::serialize(const char *id, matador::time &x)
{

}

void postgresql_statement::serialize(const char *id, basic_identifier &x)
{

}

void postgresql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{

}

std::string postgresql_statement::generate_statement_name(const matador::sql &stmt)
{
  std::stringstream name;
  name << stmt.table_name() << "_" << stmt.command();
  auto result = postgresql_statement::statement_name_map_.find(name.str());

  if (result == postgresql_statement::statement_name_map_.end()) {
    result = postgresql_statement::statement_name_map_.insert(std::make_pair(name.str(), 0)).first;
  }

  name << "_" << ++result->second;

  return name.str();
}

}
}