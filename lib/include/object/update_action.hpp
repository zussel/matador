//
// Created by sascha on 12.03.16.
//

#ifndef OOS_UPDATE_ACTION_HPP
#define OOS_UPDATE_ACTION_HPP

#include "object/action.hpp"
#include "object_serializer.hpp"

namespace oos {

class object_proxy;

/**
 * @internal
 * @class update_action
 * @brief Action when updating an serializable.
 *
 * This action is used when an objected
 * is updated on the database.
 */
class OOS_API update_action : public action
{
public:
  /**
   * Creates an update_action.
   *
   * @param o The updated serializable.
   */
  template < class T >
  update_action(object_proxy *proxy)
    : action(&backup<T>, &restore<T>)
    , proxy_(proxy)
  {}

  virtual ~update_action() {}

  virtual void accept(action_visitor *av);

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const;

  template < class T >
  static void backup(byte_buffer &buffer, action *act)
  {
    object_serializer<T> serializer;
    serializer.serialize(static_cast<update_action*>(act)->proxy_, &buffer);
  }

  template < class T >
  static void restore(byte_buffer &buffer, action *act)
  {
    // TODO: pass object store instance
    object_serializer<T> serializer;
    serializer.deserialize(static_cast<update_action*>(act)->proxy_, &buffer, nullptr);
  }

private:
  object_proxy *proxy_;
};

}

#endif //OOS_UPDATE_ACTION_HPP
