/**
 * @author sascha on 4/20/16.
 */

#ifndef OOS_HAS_MANY_ITEM_HPP
#define OOS_HAS_MANY_ITEM_HPP

#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/base_class.hpp"
#include "matador/utils/is_builtin.hpp"

#include "matador/object/basic_has_many_item.hpp"
#include "matador/object/object_ptr.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

template < class T, template <class ...> class C >
class has_many_iterator;

template < class T, template <class ...> class C >
class const_has_many_iterator;

/**
 * @class has_many_item
 * @brief Holds item and owner id of a has many relation
 * @tparam T Type of the item
 */
template < class T, class Enable = void >
class has_many_item;

template < class T >
class has_many_item<T, typename std::enable_if<!is_builtin<T>::value>::type > : public basic_has_many_item
{
public:
  typedef T object_type;
  typedef object_ptr<object_type> value_type;

public:

//  using basic_has_many_item::basic_has_many_item;

  has_many_item()
    : basic_has_many_item()
  {
    std::cout << "creating has_many_item 0 " << this << " (type: " << typeid(T).name() << ")\n";
  }

  ~has_many_item()
  {
    std::cout << "deleting has_many_item " << this << " (type: " << typeid(T).name() << ")\n";
  }

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, const object_ptr<T> &item)
    : basic_has_many_item(owner_id, item_id, id)
    , item_(item)
  {
    std::cout << "creating has_many_item 1 " << this << " (type: " << typeid(T).name() << ")\n";
  }

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, object_proxy *proxy)
    : basic_has_many_item(owner_id, item_id, id)
    , item_(value_type(proxy))
  {
    std::cout << "creating has_many_item 2 " << this << " (type: " << typeid(T).name() << ")\n";
  }

  has_many_item(const std::string &owner_id, const std::string &item_id, basic_identifier *owner)
    : basic_has_many_item(owner_id, item_id, owner)
  {
    std::cout << "creating has_many_item 3 " << this << " (type: " << typeid(T).name() << ")\n";
  }
  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<basic_has_many_item>(this));
    serializer.serialize(item_id().c_str(), item_, cascade_type::NONE);
  }

  value_type value() const
  {
    return item_;
  }

private:
  template < class V, template <class ...> class C >
  friend class has_many_iterator;
  template < class V, template <class ...> class C >
  friend class const_has_many_iterator;

  has_one<T> item_;
};

template < class T >
class has_many_item<T, typename std::enable_if<is_builtin<T>::value>::type > : public basic_has_many_item
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

  has_many_item(const std::string &owner_id, const std::string &item_id,
                const std::shared_ptr<basic_identifier> &id, object_proxy *)
    : basic_has_many_item(owner_id, item_id, id)
  {
    throw std::runtime_error("couldn't initialize builtin type with object_proxy");
  }

  has_many_item(const std::string &owner_id, const std::string &item_id, basic_identifier *owner)
      : basic_has_many_item(owner_id, item_id, owner)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<basic_has_many_item>(this));
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
  template < class V, template <class ...> class C >
  friend class has_many_iterator;
  template < class V, template <class ...> class C >
  friend class const_has_many_iterator;

  T item_;
};

/// @endcond

}

#endif //OOS_HAS_MANY_ITEM_HPP
