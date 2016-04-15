//
// Created by sascha on 1/6/16.
//

#ifndef OOS_BASIC_HAS_MANY_HPP
#define OOS_BASIC_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/abstract_has_many.hpp"
#include "object/container_type_traits.hpp"

namespace oos {

namespace detail {
class object_inserter;
}

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

template < class T, template <class ...> class C = std::vector >
class basic_has_many;

template < class T, template <class ...> class C = std::vector  >
class has_many;

template < class T, template <class ...> class C, class Enable = void >
struct has_many_iterator_traits;

template < class T, template <class ...> class C, class Enable = void >
struct const_has_many_iterator_traits;

template < class T, template <class ...> class C >
class has_many_iterator;

template < class T, template <class ...> class C >
class const_has_many_iterator;

template < class T, template <class ...> class C >
class basic_has_many : public abstract_has_many
{
public:
  typedef basic_has_many<T, C> base;
  typedef has_many_iterator<T, C> iterator;
  typedef typename iterator::value_type value_type;
  typedef typename iterator::item_type item_type;
  typedef typename iterator::container_type container_type;
  typedef typename iterator::relation_type relation_type;
  typedef const_has_many_iterator<T,C> const_iterator;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator container_iterator;

public:
  iterator begin() { return iterator(container_.begin()); }
  iterator end() { return iterator(container_.end()); }

  const_iterator begin() const { return const_iterator(container_.begin()); }
  const_iterator end() const { return const_iterator(container_.end()); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }

  void reset() { container_.clear(); }

  void append(const typename iterator::internal_type &item) { container_.push_back(item); }

  bool has_join_table() const
  {
    return true;
  }

protected:
  friend class detail::object_inserter;
  friend class object_store;

  object_proxy *owner_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;

//  detail::modified_marker mark_modified_owener_;

  std::function<void(object_store&, object_proxy*)> mark_modified_owener_;

  container_type container_;
};

}
#endif //OOS_BASIC_HAS_MANY_HPP
