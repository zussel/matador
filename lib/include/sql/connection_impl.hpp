//
// Created by sascha on 1/28/16.
//

#ifndef OOS_CONNECTION_IMPL_HPP
#define OOS_CONNECTION_IMPL_HPP


#include "sql/types.hpp"

#include <string>

namespace oos {

namespace detail {
class result_impl;
class statement_impl;
}
class sql;

class connection_impl
{
public:
  virtual ~connection_impl() {}

  virtual void open(const std::string &db) = 0;
  virtual bool is_open() const = 0;
  virtual void close() = 0;

  virtual detail::result_impl* execute(const std::string &stmt) = 0;
  virtual detail::statement_impl* prepare(const oos::sql &stmt) = 0;

  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual const char* type_string(data_type_t type) const = 0;

};

}
#endif //OOS_CONNECTION_IMPL_HPP
