#ifndef QUERY_BACKEND_PROVIDER_HPP
#define QUERY_BACKEND_PROVIDER_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace matador::query {

class connection_impl;
struct connection_info;
class dialect;

class backend_provider {
private:
  backend_provider();

public:
  struct basic_backend_service {
    virtual ~basic_backend_service() = default;
    [[nodiscard]] virtual connection_impl* create(const connection_info&) = 0;
    virtual void destroy(connection_impl*) = 0;
    [[nodiscard]] virtual const query::dialect* dialect() const = 0;
  };

  static backend_provider& instance();

  connection_impl* create_connection(const std::string &connection_type, const connection_info &info);
  void destroy_connection(const std::string &connection_type, connection_impl *c);
  const dialect& connection_dialect(const std::string &connection_type);

  void register_backend(const std::string &connection_type, std::unique_ptr<basic_backend_service> &&service);

private:
  using backends_t = std::unordered_map<std::string, std::unique_ptr<basic_backend_service>>;
  backends_t backends_;
};
}
#endif //QUERY_BACKEND_PROVIDER_HPP
