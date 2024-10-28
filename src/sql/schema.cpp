#include "matador/sql/schema.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/sql_error.hpp"

#include <stdexcept>
#include <unordered_set>

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

utils::result<void, sql_error> schema::create(connection &db) {
  std::unordered_set<std::string> created_tables;
  for (const auto &info : repository_) {
    if (created_tables.count(info.second.name) > 0) {
      continue;
    }
    for (const auto& col : info.second.prototype) {
      if (col.is_foreign_reference() && created_tables.count(col.ref_table()) == 0) {
        const auto it = repository_by_name_.find(col.ref_table());
        if (it == repository_by_name_.end()) {
          return utils::error(sql_error{sql_error_code::UNKNOWN_TABLE, "", "unknown table " + col.ref_table(), ""});
        }
        const auto res = query::create()
        .table(it->second.get().name, it->second.get().prototype.columns())
        .execute(db);
        if (res.is_error()) {
          return utils::error(res.err());
        }
        created_tables.insert(it->second.get().name);
      }
    }
    const auto res = query::create()
      .table(info.second.name, info.second.prototype.columns())
      .execute(db);
    if (res.is_error()) {
      return utils::error(res.err());
    }
    created_tables.insert(info.second.name);
  }

  return utils::ok<void>();
}

utils::result<void, sql_error> schema::drop(connection &db) {
  for (const auto &info : repository_) {
    const auto res = query::drop()
      .table(info.second.name)
      .execute(db);
    if (res.is_error()) {
      return utils::error(res.err());
    }
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