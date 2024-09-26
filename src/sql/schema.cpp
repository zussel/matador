#include "matador/sql/schema.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query.hpp"

#include <stdexcept>

namespace matador::sql {

schema::schema(std::string name)
: name_(std::move(name)) {}

std::string schema::name() const
{
  return name_;
}

const table_info& schema::attach(const std::type_index ti, const table_info& table)
{
  auto &ref = repository_.try_emplace(ti, table).first->second;
  repository_by_name_.try_emplace(ref.name, std::ref(ref));
  return ref;
}

utils::result<void, error> schema::create(connection &db) {
  for (const auto &info : repository_) {
    const auto res = query::create()
      .table(info.second.name, info.second.prototype.columns())
      .execute(db);
  }

  return utils::ok<void>();
}

utils::result<void, error> schema::drop(connection &db) {
  for (const auto &info : repository_) {
    const auto res = query::drop()
      .table(info.second.name)
      .execute(db);
  }

  return utils::ok<void>();
}

std::optional<table_info> schema::info(std::type_index ti) const
{
  const auto it = repository_.find(ti);
  if (it == repository_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::optional<table_info> schema::info(const std::string &name) const
{
  const auto it = repository_by_name_.find(name);
  if (it == repository_by_name_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::pair<std::string, std::string> schema::reference(const std::type_index &ti) const
{
  if (const auto it = repository_.find(ti); it != repository_.end()) {
    if (!it->second.prototype.has_primary_key()) {
      throw std::logic_error("table doesn't has primary key");
    }
    return { it->second.name, it->second.prototype.primary_key().value().name() };
  }

  return {};
}

bool schema::exists(const std::type_index &ti) const
{
  return repository_.count(ti) > 0;
}

schema::iterator schema::begin()
{
  return repository_.begin();
}

schema::const_iterator schema::begin() const
{
  return repository_.begin();
}

schema::iterator schema::end()
{
  return repository_.end();
}

schema::const_iterator schema::end() const
{
  return repository_.end();
}

bool schema::empty() const
{
  return repository_.empty();
}

}