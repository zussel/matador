//
// Created by sascha on 1/6/16.
//

#ifndef OOS_BASIC_HAS_MANY_HPP
#define OOS_BASIC_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/container_type_traits.hpp"

#include <vector>

namespace oos {

namespace detail {
class object_inserter;
}

/**
 * @class has_many_item
 * @brief Holds item and owner id of a has many relation
 * @tparam T Type of the item
 */
template < class T >
class has_many_item
{
public:
  typedef object_ptr<T> value_pointer;

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

  value_pointer value() const
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

template < class T, template <class ...> class C = std::vector, class Enable = void >
class has_many;

template < class T, template <class ...> class C = std::vector, class Enable = void >
class has_many_iterator;

template < class T, template <class ...> class C >
class has_many_iterator<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value>::type> : public std::iterator<std::random_access_iterator_tag, T>
{
public:
  typedef has_many_iterator<T, C> self;
  typedef object_ptr<T> value_pointer;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
  typedef typename container_type::iterator container_iterator;
public:
  has_many_iterator() {}
  explicit has_many_iterator(container_iterator iter)
    : iter_(iter)
  {}
  ~has_many_iterator() {}

  self& operator++()
  {
    ++iter_;
    return *this;
  }

  self operator++(int)
  {
    self tmp = *this;
    ++iter_;
    return tmp;
  }

  self& operator--()
  {
    self tmp = *this;
    --iter_;
    return tmp;
  }

  self operator--(int)
  {
    return self();
  }

  value_pointer operator->() const
  {
    return iter_->value();
  }

private:
//  template < class T, template <class ...> class C >
  friend class has_many<T, C>;

  container_iterator iter_;
};

template < class T, template <class ...> class C >
class basic_has_many
{
public:
  typedef basic_has_many<T, C> base;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
  typedef has_many_iterator<T,C> iterator;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator container_iterator;

  typedef value_type& reference;
  typedef value_type* pointer;

public:
  iterator begin() { return iterator(container_.begin()); }
  iterator end() { return iterator(container_.end()); }

//  const_iterator begin() const { return container_.begin(); }
//  const_iterator end() const { return container_.end(); }

protected:
  friend class detail::object_inserter;
  friend class object_store;

  object_store *ostore_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;

  container_type container_;
};

}
#endif //OOS_BASIC_HAS_MANY_HPP
