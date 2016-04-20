/**
 * @author sascha on 4/20/16.
 */

#ifndef OOS_HAS_MANY_ITEM_HPP
#define OOS_HAS_MANY_ITEM_HPP

#include "tools/basic_identifier.hpp"

#include "object/object_ptr.hpp"

#include <string>

namespace oos {

struct abstract_has_many_item {};
/**
 * @class has_many_item
 * @brief Holds item and owner id of a has many relation
 * @tparam T Type of the item
 */
template < class T, class Enable = void >
class has_many_item;

template < class T >
class has_many_item<T, typename std::enable_if<!std::is_scalar<T>::value>::type > : public abstract_has_many_item
{
public:
  typedef T object_type;
  typedef object_ptr<object_type> value_type;

public:

  has_many_item()
    : owner_id_("owner_id")
    , item_id_("item_id")
  { }

  has_many_item(const std::string &owner_id, const std::string &item_id)
    : owner_id_(owner_id)
    , item_id_(item_id)
  { }

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, const object_ptr<T> &item)
    : owner_(id)
    , item_(item)
    , owner_id_(owner_id)
    , item_id_(item_id)
  { }

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(owner_id_.c_str(), *owner_);
    serializer.serialize(item_id_.c_str(), item_, cascade_type::NONE);
  }

  value_type value() const
  {
    return item_;
  }

  void owner(basic_identifier *id)
  {
    owner_.reset(id);
  }

  std::string owner_id() const
  {
    return owner_id_;
  }

  void owner_id(const std::string &oid)
  {
    owner_id_ = oid;
  }

  std::string item_id() const
  {
    return item_id_;
  }

  void item_id(const std::string &iid)
  {
    item_id_ = iid;
  }

private:
  std::shared_ptr<basic_identifier> owner_;
  has_one<T> item_;

  std::string owner_id_;
  std::string item_id_;
};

template < class T >
class has_many_item<T, typename std::enable_if<std::is_scalar<T>::value>::type > : public abstract_has_many_item
{
public:
  typedef T value_type;

public:

  has_many_item()
    : owner_id_("owner_id")
    , item_name_("item")
  { }

  has_many_item(const std::string &owner_id, const std::string &item_name)
    : owner_id_(owner_id)
    , item_(item_name)
  { }

  has_many_item(const std::string &owner_id, const std::string &item_name,
                const std::shared_ptr<basic_identifier> &id, T item)
    : owner_(id)
    , item_(item)
    , owner_id_(owner_id)
    , item_name_(item_name)
  { }

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(owner_id_.c_str(), *owner_);
    serializer.serialize(item_name_.c_str(), item_);
  }

  value_type value()
  {
    return item_;
  }

  const value_type value() const
  {
    return item_;
  }

  std::string owner_id() const
  {
    return owner_id_;
  }

  void owner_id(const std::string &oid)
  {
    owner_id_ = oid;
  }

  std::string item_name() const
  {
    return item_name_;
  }

  void item_id(const std::string &item_name)
  {
    item_name_ = item_name;
  }

private:
  std::shared_ptr<basic_identifier> owner_;
  T item_;

  std::string owner_id_;
  std::string item_name_;
};

}

#endif //OOS_HAS_MANY_ITEM_HPP
