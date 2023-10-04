#ifndef OOS_MEMORY_CONNECTION_HPP
#define OOS_MEMORY_CONNECTION_HPP

#include "matador/sql/connection_impl.hpp"

namespace matador {

namespace memory {

/// @cond MATADOR_DEV

class memory_connection : public matador::connection_impl
{
public:
  void open(const connection_info &) override {}
  bool is_open() const override { return true; }
  void close() override {}

  matador::detail::result_impl *execute(const matador::sql &) override { return nullptr; }
  matador::detail::result_impl *execute(const std::string &) override { return nullptr; }
  detail::statement_impl *prepare(const matador::sql &) override { return nullptr; };

  void begin() override {}

  void commit() override {}

  void rollback() override {}

  std::string type() const override { return "memory"; };
  version client_version() const override { return {0, 8, 1}; };
  version server_version() const override { return {0, 8, 1}; };

  virtual const char *type_string(database_type) const { return nullptr; }

  bool exists(const std::string &) override { return true; }

  std::vector<field> describe(const std::string &) override { return {}; }

  basic_dialect* dialect() override { return nullptr; }

  unsigned short default_port() const override { return 0; }
};

/// @endcond

}
}
#endif //OOS_MEMORY_CONNECTION_HPP
