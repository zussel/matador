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
  virtual void open() {}
  virtual bool is_open() const { return true; }
  virtual void close() {}

  virtual oos::detail::result_impl *execute(const std::string &) { return nullptr; }
};

}
}
#endif //OOS_MEMORY_CONNECTION_HPP
