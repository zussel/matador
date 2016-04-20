//
// Created by sascha on 12.03.16.
//

#ifndef OOS_UPDATE_ACTION_HPP
#define OOS_UPDATE_ACTION_HPP

#include "object/action.hpp"
//#include "object/object_serializer.hpp"
#include "object/delete_action.hpp"

namespace oos {

class object_proxy;
class object_serializer;
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
  update_action(object_proxy *proxy, T *obj)
    : action(&backup<T, object_serializer>, &restore<T, object_serializer>)
    , proxy_(proxy)
    , delete_action_(new delete_action(proxy, obj))
  {}

  virtual void accept(action_visitor *av);

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const;

  template < class T, class S >
  static void backup(byte_buffer &buffer, action *act, S &serializer)
  {
//    object_serializer serializer;
    T* obj = (T*)(static_cast<update_action*>(act)->proxy_->obj());
    serializer.serialize(obj, &buffer);
  }

  template < class T, class S >
  static void restore(byte_buffer &buffer, action *act, object_store *store, S &serializer)
  {
//    object_serializer serializer;
    T* obj = (T*)(static_cast<update_action*>(act)->proxy_->obj());
    serializer.deserialize(obj, &buffer, store);
  }

  delete_action* release_delete_action();

private:
  object_proxy *proxy_;
  std::unique_ptr<delete_action> delete_action_;
};

}

#endif //OOS_UPDATE_ACTION_HPP
