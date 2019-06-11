//
// Created by sascha on 03.06.19.
//

#include "matador/db/postgresql/postgresql_statement.hpp"
#include "matador/db/postgresql/postgresql_connection.hpp"

#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/identifier.hpp"

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

  host_strings_.resize(host_size);
  host_params_.resize(host_size);

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

template < class T >
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, T &x)
{
  auto strval = std::to_string(x);
  strings[index] = strval;
  params[index++] = strval.c_str();
}

void bind_null(size_t index)
{

}

void postgresql_statement::serialize(const char *, char &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, short &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, int &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, long &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, unsigned char &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, unsigned short &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, unsigned int &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, unsigned long &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, float &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, double &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, bool &x)
{
  bind_value(host_strings_, host_params_, host_size, x);
}

void postgresql_statement::serialize(const char *, char *x, size_t s)
{
  host_params_[host_size++] = x;
}

void postgresql_statement::serialize(const char *, varchar_base &x)
{
  host_strings_[host_size] = x.str();
  host_params_[host_size++] = x.str().c_str();
}

void postgresql_statement::serialize(const char *, std::string &x)
{
  host_strings_[host_size] = x;
  host_params_[host_size++] = x.c_str();
}

void postgresql_statement::serialize(const char *id, matador::date &x)
{

}

void postgresql_statement::serialize(const char *id, matador::time &x)
{

}

void postgresql_statement::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

void postgresql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    bind_null(host_index++);
  }
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