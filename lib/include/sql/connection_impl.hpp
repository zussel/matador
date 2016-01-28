//
// Created by sascha on 1/28/16.
//

#ifndef OOS_CONNECTION_IMPL_HPP
#define OOS_CONNECTION_IMPL_HPP

#include <string>
#include <database/statement_impl.hpp>

namespace oos {

namespace detail {
class result_impl;
}

class connection_impl
{
public:
  virtual ~connection_impl() {}

  virtual void open() = 0;
  virtual bool is_open() const = 0;
  virtual void close() = 0;

  virtual detail::result_impl* execute(const std::string &smt) = 0;
  virtual detail::statement_impl* prepare(const std::string &smt) = 0;

  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;
};

}
#endif //OOS_CONNECTION_IMPL_HPP
