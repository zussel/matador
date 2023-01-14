#ifndef OOS_MEMORY_CONNECTION_HPP
#define OOS_MEMORY_CONNECTION_HPP

#include "matador/sql/connection_impl.hpp"

namespace matador {

namespace memory {

/// @cond MATADOR_DEV

class memory_connection : public matador::connection_impl
{
public:
  void open(const std::string &) override {}
  bool is_open() const override { return true; }
  void close() override {}

  matador::detail::result_impl *execute(const matador::sql &) override { return nullptr; }
  matador::detail::result_impl *execute(const std::string &) override { return nullptr; }
  detail::statement_impl *prepare(const matador::sql &) override { return nullptr; };

  void begin() override {}

  void commit() override {}

  void rollback() override {}

  std::string type() const override { return "memory"; };
  std::string client_version() const override { return "0.6.1"; };
  std::string server_version() const override { return "0.6.1"; };

  virtual const char *type_string(database_type) const { return nullptr; }

  bool exists(const std::string &) override { return true; }

  std::vector<field> describe(const std::string &) override { return std::vector<field>(); }

  basic_dialect* dialect() override { return nullptr; }
};

/// @endcond

}
}
#endif //OOS_MEMORY_CONNECTION_HPP
