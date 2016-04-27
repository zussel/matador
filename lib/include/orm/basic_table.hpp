//
// Created by sascha on 3/24/16.
//

#ifndef OOS_BASIC_TABLE_HPP
#define OOS_BASIC_TABLE_HPP

#include <string>
#include <functional>

namespace oos {

class connection;
class object_proxy;
class object_store;
class persistence;

class basic_table
{
public:
  basic_table(const std::string &name, persistence &p);

  virtual ~basic_table();

  std::string name() const;
  virtual void create(connection &conn) = 0;
  virtual void drop(connection &conn) = 0;
  virtual void load(object_store &p) = 0;
  virtual void prepare(connection &conn) = 0;
  virtual void insert(object_proxy *proxy) = 0;
  virtual void update(object_proxy *proxy) = 0;
  virtual void remove(object_proxy *proxy) = 0;

protected:
  persistence &persistence_;

private:
  std::string name_;
};

}

#endif //OOS_BASIC_TABLE_HPP
