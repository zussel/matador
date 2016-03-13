//
// Created by sascha on 12.03.16.
//

#ifndef OOS_UPDATE_ACTION_HPP
#define OOS_UPDATE_ACTION_HPP

#include "object/action.hpp"

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
  update_action(object_proxy *proxy)
    : action(backup, restore)
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
//    object_serializer serializer;
//    serializer.serialize(obj, &buffer);
  }

  template < class T >
  static void restore(byte_buffer &, action *act)
  {
  }

private:
  object_proxy *proxy_;
};

}

#endif //OOS_UPDATE_ACTION_HPP
