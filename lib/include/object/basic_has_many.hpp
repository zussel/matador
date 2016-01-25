//
// Created by sascha on 1/6/16.
//

#ifndef OOS_BASIC_HAS_MANY_HPP
#define OOS_BASIC_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/container_type_traits.hpp"

namespace oos {

namespace detail {
class object_inserter;
}

/**
 * @class has_many_item
 * @brief Holds item and owner id of a has many relation
 * @tparam T Type of the item
 */
template < class T, class Enable = void >
class has_many_item;

template < class T >
class has_many_item<T, typename std::enable_if<!std::is_scalar<T>::value>::type >
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
class has_many_item<T, typename std::enable_if<std::is_scalar<T>::value>::type >
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

template < class T, template <class ...> class C = std::vector >
class basic_has_many;

template < class T, template <class ...> class C = std::vector, class Enable = void  >
class has_many;

template < class T, template <class ...> class C, class Enable = void >
class has_many_iterator;

template < class T, template <class ...> class C, class Enable = void >
class const_has_many_iterator;


class abstract_has_many
{
public:
  void owner_field(const std::string &ownerfield) { owner_field_ = ownerfield; }
  void item_field(const std::string &itemfield) { item_field_ = itemfield; }

  std::string owner_field() const { return owner_field_; }
  std::string item_field() const { return item_field_; }

protected:
  friend class detail::object_inserter;

  object_store *ostore_ = nullptr;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";
};

template < class T, template <class ...> class C >
class basic_has_many : public abstract_has_many
{
public:
  typedef basic_has_many<T, C> base;
  typedef has_many_iterator<T, C> iterator;
  typedef typename iterator::container_type container_type;
  typedef typename iterator::item_ptr item_ptr;
  typedef const_has_many_iterator<T,C> const_iterator;
  typedef typename container_type::size_type size_type;

public:
  iterator begin() { return iterator(container_.begin()); }
  iterator end() { return iterator(container_.end()); }

  const_iterator begin() const { return const_iterator(container_.begin()); }
  const_iterator end() const { return const_iterator(container_.end()); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }

  iterator erase(iterator i)
  {
    return iterator(container_.erase(i.iter_));
  }
  iterator erase(iterator start, iterator end)
  {
    return iterator(container_.erase(start.iter_, end.iter_));
  }

  bool has_join_table() const
  {
    return true;
  }

protected:
  friend class detail::object_inserter;
  friend class object_store;

  std::shared_ptr<basic_identifier> owner_id_;

  container_type container_;
};

}
#endif //OOS_BASIC_HAS_MANY_HPP
