//
// Created by sascha on 03.06.19.
//

#include "matador/db/postgresql/postgresql_exception.hpp"
#include "matador/db/postgresql/postgresql_statement.hpp"
#include "matador/db/postgresql/postgresql_prepared_result.hpp"
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

  res_ = PQprepare(db.handle(), name_.c_str(), str().c_str(), host_size, nullptr);
  if (res_ == nullptr) {
    THROW_POSTGRESQL_ERROR(db_.handle(), "execute", "error on sql statement");
  } else if (PQresultStatus(res_) != PGRES_COMMAND_OK) {
    THROW_POSTGRESQL_ERROR(db_.handle(), "execute", "error on sql statement");
  }
}

postgresql_statement::postgresql_statement(const postgresql_statement &x)
  : statement_impl(x)
  , db_(x.db_)
  , result_size(x.result_size)
  , host_size(x.host_size)
  , host_strings_(x.host_strings_)
  , host_params_(x.host_params_)
  , name_(x.name_)
{
  if (res_ != nullptr) {
    PQclear(res_);
  }
  res_ = x.res_;
}

postgresql_statement &postgresql_statement::operator=(const postgresql_statement &x)
{
  db_ = x.db_;
  result_size = x.result_size;
  host_index = x.host_index;
  host_size = x.host_size;
  host_strings_ = x.host_strings_;
  host_params_ = x.host_params_;
  name_ = x.name_;

  if (res_ != nullptr) {
    PQclear(res_);
  }
  res_ = x.res_;
  return *this;
}

postgresql_statement::~postgresql_statement()
{
  clear();
}

void postgresql_statement::clear()
{
  if (res_ != nullptr) {
    PQclear(res_);
    res_ = nullptr;
  }

  host_params_.clear();
  host_strings_.clear();
}

detail::result_impl *postgresql_statement::execute()
{
  PGresult *res = PQexecPrepared(db_.handle(), name_.c_str(), host_size, host_params_.data(), nullptr, nullptr, 0);
  auto status = PQresultStatus(res);
  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
    THROW_POSTGRESQL_ERROR(db_.handle(), "execute", "error on sql statement");
  }
  return new postgresql_prepared_result(this, res);
}

void postgresql_statement::reset()
{
}

template < class T >
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, T &x)
{
  strings[index] = std::to_string(x);
  params[index] = strings[index].c_str();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, char &x)
{
  strings[index] = x;
  params[index] = strings[index].data();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, matador::date &x)
{
  strings[index] = matador::to_string(x, date_format::ISO8601);
  params[index] = strings[index].c_str();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, matador::time &x)
{
  strings[index] = matador::to_string(x, "%Y-%m-%d %T.%f");
  params[index] = strings[index].c_str();
  ++index;
}

void postgresql_statement::serialize(const char *id, char &x)
{
  std::cout << "PSQL: Bind value (char) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, short &x)
{
  std::cout << "PSQL: Bind value (short) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, int &x)
{
  std::cout << "PSQL: Bind value (int) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, long &x)
{
  std::cout << "PSQL: Bind value (long) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, unsigned char &x)
{
  std::cout << "PSQL: Bind value (unsigned char) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, unsigned short &x)
{
  std::cout << "PSQL: Bind value (unsigned short) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, unsigned int &x)
{
  std::cout << "PSQL: Bind value (unsigned int) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, unsigned long &x)
{
  std::cout << "PSQL: Bind value (unsigned long) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, float &x)
{
  std::cout << "PSQL: Bind value (float) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, double &x)
{
  std::cout << "PSQL: Bind value (double) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, bool &x)
{
  std::cout << "PSQL: Bind value (bool) [" << x << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, char *x, size_t)
{
  std::cout << "PSQL: Bind value (char*) [" << x << "] for column " << id << "\n";
  host_params_[host_index++] = x;
}

void postgresql_statement::serialize(const char *id, varchar_base &x)
{
  std::cout << "PSQL: Bind value (varchar) [" << x << "] for column " << id << "\n";
  host_strings_[host_index] = x.str();
  host_params_[host_index] = host_strings_[host_index].c_str();
  ++host_index;
}

void postgresql_statement::serialize(const char *id, std::string &x)
{
  std::cout << "PSQL: Bind value (string) [" << x << "] for column " << id << "\n";
  host_strings_[host_index] = x;
  host_params_[host_index] = host_strings_[host_index].c_str();
  ++host_index;
}

void postgresql_statement::serialize(const char *id, matador::date &x)
{
  std::cout << "PSQL: Bind value (date) [" << matador::to_string(x) << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, matador::time &x)
{
  std::cout << "PSQL: Bind value (time) [" << matador::to_string(x) << "] for column " << id << "\n";
  bind_value(host_strings_, host_params_, host_index, x);
}

void postgresql_statement::serialize(const char *id, basic_identifier &x)
{
  std::cout << "PSQL: Bind value (identifier) [" << x << "] for column " << id << "\n";
  x.serialize(id, *this);
}

void postgresql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  std::cout << "PSQL: Bind value (object) for column " << id << "\n";
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    host_params_[host_index++] = nullptr;
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