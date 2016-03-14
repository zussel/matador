//
// Created by sascha on 12.03.16.
//

#ifndef OOS_DELETE_ACTION_HPP
#define OOS_DELETE_ACTION_HPP

#include "object/action.hpp"
#include "object/object_proxy.hpp"
#include "object/object_serializer.hpp"

#include "tools/basic_identifier.hpp"

namespace oos {

/**
 * @internal
 * @class delete_action
 * @brief Action when deleting an serializable.
 *
 * This action is used when an objected
 * is deleted from the database.
 */
class OOS_API delete_action : public action
{
public:
  /**
   * Creates an delete_action.
   *
   * @param classname The serializable type name.
   * @param id The id of the deleted serializable.
   */
  template < class T >
  delete_action(const char *classname, unsigned long id, basic_identifier *pk, T*)
    : action(&backup<T>, &restore<T>)
    , classname_(classname)
    , id_(id)
    , pk_(pk)
  {}

  virtual ~delete_action();

  virtual void accept(action_visitor *av);

  /**
   * Return the class name of the
   * serializable.
   *
   * @return The class name.
   */
  const char* classname() const;

  /**
   * The primary key of the serializable of the action.
   *
   * @return The primary key of the deleted serializable.
   */
  basic_identifier* pk() const;

  unsigned long id() const;

  template < class T >
  static void backup(byte_buffer &buffer, action *act)
  {
  }

  template < class T >
  static void restore(byte_buffer &buffer, action *act, object_store *store)
  {
    // TODO: pass object store instance
    // check if there is an serializable with id in
    // serializable store
    delete_action *da(static_cast<delete_action*>(act));
    object_proxy *proxy = delete_action::find_proxy(store, da->id());
    if (!proxy) {
      // create proxy
      proxy = new object_proxy(new T, da->id(), store);
      delete_action::insert_proxy(store, proxy);
    }
    object_serializer serializer;
    if (!proxy->obj()) {
      // create serializable with id and deserialize
      proxy->reset(new T);
      // data from buffer into serializable
      serializer.deserialize<T>((T*)proxy->obj(), &buffer, store);
      // restore pk
      if (da->pk()) {
        proxy->pk().reset(da->pk()->clone());
      }
      // insert serializable
//      store->insert<T>(proxy, false);
    } else {
      // data from buffer into serializable
      serializer.deserialize<T>((T*)proxy->obj(), &buffer, store);
    }
  }
private:
  static object_proxy* find_proxy(object_store *store, unsigned long id);
  static void insert_proxy(object_store *store, object_proxy *proxy);

private:
  std::string classname_;
  unsigned long id_;
  std::unique_ptr<basic_identifier> pk_;
};

}

#endif //OOS_DELETE_ACTION_HPP
