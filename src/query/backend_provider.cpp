#include "matador/query/backend_provider.hpp"

#include "matador/query/interface/connection_impl.hpp"

#include "matador/utils/library.hpp"

#include <algorithm>
#include <stdexcept>

namespace {
class backend_service final : public matador::query::backend_provider::basic_backend_service {
public:
  explicit backend_service(const std::string &connection_type);
  backend_service(const backend_service&) = delete;
  backend_service& operator=(const backend_service&) = delete;
  backend_service(backend_service&&) noexcept = default;
  backend_service& operator=(backend_service&&) noexcept = default;
  ~backend_service() override;

  [[nodiscard]] matador::query::connection_impl* create(const matador::query::connection_info&) override;
  void destroy(matador::query::connection_impl *conn) override;
  [[nodiscard]] const class matador::query::dialect* dialect() const override;

private:
  typedef matador::query::connection_impl*(*create_func)(const matador::query::connection_info&);
  typedef void (*destroy_func)(matador::query::connection_impl*);
  typedef const class matador::query::dialect*(*dialect_func)();

  create_func create_connection{};
  destroy_func destroy_connection{};
  dialect_func get_dialect{};
  matador::utils::library lib;
};
}
namespace matador::query {
backend_provider::backend_provider() = default;
// {
// backends_.emplace("noop", std::make_unique<noop_backend_context>());
// }

backend_provider &backend_provider::instance() {
  static backend_provider provider;
  return provider;
}

connection_impl *backend_provider::create_connection(const std::string &connection_type, const connection_info &info)
{
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_service>(connection_type)).first;
  }
  return it->second->create(info);
}

void backend_provider::destroy_connection(const std::string &connection_type, connection_impl *c)
{
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_service>(connection_type)).first;
  }
  return it->second->destroy(c);
}

const dialect &backend_provider::connection_dialect(const std::string &connection_type) {
  auto it = backends_.find(connection_type);
  if (it == backends_.end()) {
    it = backends_.emplace(connection_type, std::make_unique<backend_service>(connection_type)).first;
  }
  return *it->second->dialect();
}
void backend_provider::register_backend(const std::string &connection_type, std::unique_ptr<basic_backend_service> &&service) {
  backends_.emplace(connection_type, std::move(service));
}
}

namespace {
backend_service::backend_service(const std::string &connection_type)
{
  if (!lib.load("matador-" + connection_type)) {
    throw std::runtime_error("couldn't load library '" + connection_type + "'");
  }

  create_connection = reinterpret_cast<create_func>(reinterpret_cast<std::uintptr_t>(lib.function("create_database")));
  destroy_connection = reinterpret_cast<destroy_func>(reinterpret_cast<std::uintptr_t>(lib.function("destroy_database")));
  get_dialect = reinterpret_cast<dialect_func >(reinterpret_cast<std::uintptr_t>(lib.function("get_dialect")));
}

matador::query::connection_impl *backend_service::create(const matador::query::connection_info &info) {
  return (create_connection)(info);
}

void backend_service::destroy(matador::query::connection_impl *conn)
{
  (destroy_connection)(conn);
}

const matador::query::dialect *backend_service::dialect() const
{
  return (get_dialect)();
}

backend_service::~backend_service() {
  lib.unload();
}
}
