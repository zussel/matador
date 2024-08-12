#include "matador/sql/backend_provider.hpp"
#include "matador/sql/noop_connection.hpp"
#include "matador/sql/dialect_builder.hpp"

#include <algorithm>
#include <stdexcept>

namespace matador::sql {
backend_provider::backend_provider()
{
  backends_.emplace("noop", std::make_unique<noop_backend_context>());
}

backend_provider &backend_provider::instance() {
  static backend_provider provider;
  return provider;
}

connection_impl *backend_provider::create_connection(const std::string &connection_type, const connection_info &info)
{
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_context>(connection_type)).first;
  }
  return it->second->create(info);
}

void backend_provider::destroy_connection(const std::string &connection_type, connection_impl *c)
{
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_context>(connection_type)).first;
  }
  return it->second->destroy(c);
}

const dialect &backend_provider::connection_dialect(const std::string &connection_type)
{
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_context>(connection_type)).first;
  }
  return *it->second->dialect();
}

backend_provider::backend_context::backend_context(const std::string &connection_type)
{
  if (!lib.load("matador-" + connection_type)) {
    throw std::runtime_error("couldn't load library '" + connection_type + "'");
  }

  create_connection = reinterpret_cast<create_func>(reinterpret_cast<std::uintptr_t>(lib.function("create_database")));
  destroy_connection = reinterpret_cast<destroy_func>(reinterpret_cast<std::uintptr_t>(lib.function("destroy_database")));
  get_dialect = reinterpret_cast<dialect_func >(reinterpret_cast<std::uintptr_t>(lib.function("get_dialect")));
}

connection_impl *backend_provider::backend_context::create(const connection_info &info)
{
  return (create_connection)(info);
}

void backend_provider::backend_context::destroy(connection_impl *conn)
{
  (destroy_connection)(conn);
}

const class dialect *backend_provider::backend_context::dialect() const
{
  return (get_dialect)();
}

backend_provider::backend_context::~backend_context()
{
  lib.unload();
}

connection_impl *backend_provider::noop_backend_context::create(const connection_info &info)
{
  return noop_connections_.insert(std::make_unique<noop_connection>(info)).first->get();
}

void backend_provider::noop_backend_context::destroy(connection_impl *impl)
{
  auto it = std::find_if(noop_connections_.begin(), noop_connections_.end(), [impl](const auto &item) {
    return impl == item.get();
  });
  if (it != noop_connections_.end()) {
    noop_connections_.erase(it);
  }
}

const dialect *backend_provider::noop_backend_context::dialect() const
{
  static sql::dialect dialect_ = dialect_builder::builder().create().build();
  return &dialect_;
}

}