//
// Created by sascha on 10/27/15.
//

#ifndef OOS_OBJECT_INSERTER_HPP
#define OOS_OBJECT_INSERTER_HPP

#include "object/object_atomizer.hpp"

#include <set>

namespace oos {

class object_proxy;

class object_inserter : public generic_object_reader<object_inserter>
{
private:
  typedef std::set<object_proxy*> t_object_proxy_set;

public:
  typedef t_object_proxy_set::iterator iterator;             /**< Shortcut the serializable map iterator */
  typedef t_object_proxy_set::const_iterator const_iterator; /**< Shortcut the serializable map const_iterator */

  /**
   * Creates an instance of the object_deleter
   */
  object_inserter();
  virtual ~object_inserter();

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the first deletable serializable.
   */
  iterator begin();

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the last deletable serializable.
   */
  iterator end();

  template < class T >
  void read_value(const char*, const T&) {}

  void read_value(const char*, char*, int) {}
  void read_value(const char*, object_base_ptr &x);
  void read_value(const char*, object_container &x);

private:
  t_object_proxy_set object_proxies_;
};

}

#endif //OOS_OBJECT_INSERTER_HPP
