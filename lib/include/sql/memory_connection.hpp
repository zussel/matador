//
// Created by sascha on 1/28/16.
//

#ifndef OOS_MEMORY_CONNECTION_HPP
#define OOS_MEMORY_CONNECTION_HPP

#include "sql/connection_impl.hpp"

namespace oos {

namespace memory {

class memory_connection : public oos::connection_impl
{
public:
  virtual void open(const std::string &) {}
  virtual bool is_open() const { return true; }
  virtual void close() {}

  virtual oos::detail::result_impl *execute(const oos::sql &) { return nullptr; }
  virtual oos::detail::result_impl *execute(const std::string &) { return nullptr; }
  virtual detail::statement_impl *prepare(const oos::sql &) { return nullptr; };

  virtual void begin() override {}

  virtual void commit() override {}

  virtual void rollback() override {}

  virtual const char *type_string(data_type_t) const { return nullptr; }

  virtual bool exists(const std::string &) override { return true; }

  virtual basic_dialect* dialect() override { return nullptr; }
};

}
}
#endif //OOS_MEMORY_CONNECTION_HPP
