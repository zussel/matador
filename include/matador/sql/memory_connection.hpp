//
// Created by sascha on 1/28/16.
//

#ifndef OOS_MEMORY_CONNECTION_HPP
#define OOS_MEMORY_CONNECTION_HPP

#include "matador/sql/connection_impl.hpp"

namespace matador {

namespace memory {

/// @cond MATADOR_DEV

class memory_connection : public matador::connection_impl
{
public:
  virtual void open(const std::string &) override {}
  virtual bool is_open() const override { return true; }
  virtual void close() override {}

  virtual matador::detail::result_impl *execute(const matador::sql &) override { return nullptr; }
  virtual matador::detail::result_impl *execute(const std::string &) override { return nullptr; }
  virtual detail::statement_impl *prepare(const matador::sql &) override { return nullptr; };

  virtual void begin() override {}

  virtual void commit() override {}

  virtual void rollback() override {}

  virtual std::string type() const override { return "memory"; };
  virtual std::string version() const override { return "0.5.0"; };

  virtual const char *type_string(data_type) const { return nullptr; }

  virtual bool exists(const std::string &) override { return true; }

  virtual std::vector<field> describe(const std::string &) override { return std::vector<field>(); }

  virtual basic_dialect* dialect() override { return nullptr; }
};

/// @endcond

}
}
#endif //OOS_MEMORY_CONNECTION_HPP
