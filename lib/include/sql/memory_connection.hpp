//
// Created by sascha on 1/28/16.
//

#ifndef OOS_MEMORY_CONNECTION_HPP
#define OOS_MEMORY_CONNECTION_HPP

#include "sql/connection_impl.hpp"

namespace oos {

namespace memory {

/// @cond OOS_DEV

class memory_connection : public oos::connection_impl
{
public:
  virtual void open(const std::string &) override {}
  virtual bool is_open() const override { return true; }
  virtual void close() override {}

  virtual oos::detail::result_impl *execute(const oos::sql &) override { return nullptr; }
  virtual oos::detail::result_impl *execute(const std::string &) override { return nullptr; }
  virtual detail::statement_impl *prepare(const oos::sql &) override { return nullptr; };

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
