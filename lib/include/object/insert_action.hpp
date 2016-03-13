//
// Created by sascha on 12.03.16.
//

#ifndef OOS_INSERT_ACTION_HPP
#define OOS_INSERT_ACTION_HPP

#include "object/action.hpp"

namespace oos {

class object_proxy;

/**
 * @internal
 * @class insert_action
 * @brief Action when inserting an serializable.
 *
 * This action is used when an objected
 * is inserted into the database.
 */
class OOS_API insert_action : public action
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;
  typedef object_proxy_list_t::iterator iterator;
  typedef object_proxy_list_t::const_iterator const_iterator;

public:
/**
 * Creates an insert_action.
 *
 * @param type The type of the expected objects
 */
  explicit insert_action(const std::string &type);

  virtual ~insert_action();

  virtual void accept(action_visitor *av);

/**
 * Return the object type
 * of the action.
 *
 * @return The object type of the action
 */
  std::string type() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool empty() const;

  iterator find(unsigned long id);
  const_iterator find(unsigned long id) const;

  void push_back(object_proxy *proxy);

  iterator erase(iterator i);

  template < class T >
  static void backup(byte_buffer &, action *) { }

  template < class T >
  static void restore(byte_buffer &, action *act)
  {
    // remove serializable from serializable store
    insert_action *ia(static_cast<insert_action*>(act));
    for (insert_action::iterator i = ia->begin(); i != ia->end(); ++i) {
//      ia->store()->remove()
//      ostore_->remove_object(*i, false);
    }

  }

private:
  std::string type_;
  object_proxy_list_t object_proxy_list_;
};

}

#endif //OOS_INSERT_ACTION_HPP
