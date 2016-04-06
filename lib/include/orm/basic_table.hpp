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

class basic_table
{
public:
  typedef std::function<void(const std::string&, connection &)> t_create_func;
  typedef std::function<void(const std::string&, connection &)> t_drop_func;
  typedef std::function<void(connection &)> t_prepare_func;
  typedef std::function<void(object_proxy*)> t_object_func;
  typedef std::function<void()> t_void_func;

public:
  basic_table(const std::string &name, t_create_func create_func, t_drop_func drop_func,
              t_object_func insert_object_func, t_object_func update_object_func,
              t_object_func delete_object_func, t_prepare_func prepare_func,
              t_void_func destroy_func);

  ~basic_table();

  std::string name() const;
  void create(connection &conn);
  void drop(connection &conn);
  void prepare(connection &conn);
  void insert(object_proxy *proxy);
  void update(object_proxy *proxy);
  void remove(object_proxy *proxy);

private:
  std::string name_;

  t_create_func create_;
  t_drop_func drop_;
  t_prepare_func prepare_;
  t_object_func insert_object_;
  t_object_func update_object_;
  t_object_func delete_object_;
  t_void_func destroy_func_;
};

}

#endif //OOS_BASIC_TABLE_HPP
