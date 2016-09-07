/**
 * @author sascha on 4/20/16.
 */

#ifndef OOS_HAS_MANY_ITEM_HPP
#define OOS_HAS_MANY_ITEM_HPP

#include "tools/basic_identifier.hpp"
#include "tools/base_class.hpp"

#include "object/basic_has_many_item.hpp"
#include "object/object_ptr.hpp"

#include <string>

namespace oos {

/**
 * @class has_many_item
 * @brief Holds item and owner id of a has many relation
 * @tparam T Type of the item
 */
template < class T, class Enable = void >
class has_many_item;

template < class T >
class has_many_item<T, typename std::enable_if<!std::is_scalar<T>::value>::type > : public basic_has_many_item
{
public:
  typedef T object_type;
  typedef object_ptr<object_type> value_type;

public:

  using basic_has_many_item::basic_has_many_item;

  has_many_item()
    : basic_has_many_item()
  {}

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, const object_ptr<T> &item)
    : basic_has_many_item(owner_id, item_id, id)
    , item_(item)
  { }

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*oos::base_class<basic_has_many_item>(this));
    serializer.serialize(item_id().c_str(), item_, cascade_type::NONE);
  }

  value_type value() const
  {
    return item_;
  }

private:
  has_one<T> item_;
};

template < class T >
class has_many_item<T, typename std::enable_if<std::is_scalar<T>::value>::type > : public basic_has_many_item
{
public:
  typedef T object_type;
  typedef T value_type;

public:

  using basic_has_many_item::basic_has_many_item;

  has_many_item()
    : basic_has_many_item()
  { }

  has_many_item(const std::string &owner_id, const std::string &item_id)
    : basic_has_many_item(owner_id, item_id)
  { }

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, T item)
    : basic_has_many_item(owner_id, item_id, id)
    , item_(item)
  { }

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*oos::base_class<basic_has_many_item>(this));
    serializer.serialize(item_id().c_str(), item_);
  }

  value_type value()
  {
    return item_;
  }

  const value_type value() const
  {
    return item_;
  }
private:
  T item_;
};

}

#endif //OOS_HAS_MANY_ITEM_HPP
