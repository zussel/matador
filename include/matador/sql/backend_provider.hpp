#ifndef QUERY_BACKEND_PROVIDER_HPP
#define QUERY_BACKEND_PROVIDER_HPP

#include "matador/utils/library.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace matador::sql {

class connection_impl;
struct connection_info;
class dialect;

class backend_provider
{
private:
  backend_provider();

public:
  static backend_provider& instance();

  connection_impl* create_connection(const std::string &connection_type, const connection_info &info);
  void destroy_connection(const std::string &connection_type, connection_impl *c);
  const dialect& connection_dialect(const std::string &connection_type);

private:
  struct basic_backend_context {
    virtual ~basic_backend_context() = default;
    [[nodiscard]] virtual connection_impl* create(const connection_info&) = 0;
    virtual void destroy(connection_impl*) = 0;
    [[nodiscard]] virtual const sql::dialect* dialect() const = 0;
  };

  class backend_context final : public basic_backend_context {
  public:
    explicit backend_context(const std::string &connection_type);
    backend_context(const backend_context&) = delete;
    backend_context& operator=(const backend_context&) = delete;
    backend_context(backend_context&&) noexcept = default;
    backend_context& operator=(backend_context&&) noexcept = default;
    ~backend_context() override;

    [[nodiscard]] connection_impl* create(const connection_info&) override;
    void destroy(connection_impl *conn) override;
    [[nodiscard]] const class dialect* dialect() const override;

  private:
    typedef connection_impl*(*create_func)(const connection_info&);
    typedef void (*destroy_func)(connection_impl*);
    typedef const class dialect*(*dialect_func)();

    create_func create_connection{};
    destroy_func destroy_connection{};
    dialect_func get_dialect{};
    utils::library lib;
  };

  class noop_backend_context final : public basic_backend_context {
  public:
    connection_impl *create(const connection_info &info) override;
    void destroy(connection_impl *impl) override;
    [[nodiscard]] const sql::dialect *dialect() const override;

  private:
    std::unordered_set<std::unique_ptr<connection_impl>> noop_connections_;
  };

private:
  using backends_t = std::unordered_map<std::string, std::unique_ptr<basic_backend_context>>;
  backends_t backends_;
};
}
#endif //QUERY_BACKEND_PROVIDER_HPP
