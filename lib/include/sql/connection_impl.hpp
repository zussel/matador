//
// Created by sascha on 1/28/16.
//

#ifndef OOS_CONNECTION_IMPL_HPP
#define OOS_CONNECTION_IMPL_HPP


#include "sql/types.hpp"
#include "field.hpp"

#include <string>
#include <vector>

namespace oos {

namespace detail {
class result_impl;
class statement_impl;
}
class sql;
class basic_dialect;

class connection_impl
{
public:
  virtual ~connection_impl() {}

  virtual void open(const std::string &db) = 0;
  virtual bool is_open() const = 0;
  virtual void close() = 0;

  virtual detail::result_impl* execute(const oos::sql &stmt) = 0;
  virtual detail::result_impl* execute(const std::string &stmt) = 0;
  virtual detail::statement_impl* prepare(const oos::sql &stmt) = 0;

  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual bool exists(const std::string &tablename) = 0;
  virtual std::vector<field> describe(const std::string &table) = 0;

  virtual basic_dialect* dialect() = 0;
};

}
#endif //OOS_CONNECTION_IMPL_HPP
