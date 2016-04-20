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
template < class T >
class OOS_API update_action : public action
{
public:
  /**
   * Creates an update_action.
   *
   * @param o The updated serializable.
   */
  update_action(object_proxy *proxy)
    : proxy_(proxy)
    , delete_action_(new delete_action(proxy))
  {}

  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const
  {
    return proxy_;
  }

  virtual void backup(byte_buffer &buffer)
  {
    T* obj = (T*)(proxy_->obj());
    serializer_.serialize(obj, &buffer);
  }

  virtual void restore(byte_buffer &buffer, object_store *store)
  {
//    object_serializer serializer;
    T* obj = (T*)(proxy_->obj());
    serializer_.deserialize(obj, &buffer, store);
  }

  delete_action* release_delete_action()
  {
    return delete_action_.release();
  }

private:
  object_proxy *proxy_;
  std::unique_ptr<delete_action> delete_action_;
};

}

#endif //OOS_UPDATE_ACTION_HPP
