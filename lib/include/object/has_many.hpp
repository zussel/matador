//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_MANY_HPP
#define OOS_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/container_type_traits.hpp"
#include "object/object_store.hpp"

#include <vector>
#include <list>
#include <memory>

namespace oos {

class object_store;

template < class T >
class basic_has_many
{
public:
  typedef has_one<T> value_type;
  typedef value_type& reference;
  typedef value_type* pointer;

protected:
  friend class object_inserter;

  object_store *ostore_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;
};

template < class T, template <class ...> class C = std::vector, class Enable = void >
class has_many;

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
    serializer.serialize(item_id_.c_str(), item_);
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
  container_iterator iter_;
};

/**
 * has_many with std::vector
 */
template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value>::type> : public basic_has_many<T>
{
public:

  typedef basic_has_many<T> base;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> value_type;
//  typedef oos::has_one<T> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
//  typedef C<oos::has_one<T>, std::allocator<oos::has_one<T>>> container_type;
  typedef has_many_iterator<T,C> iterator;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator container_iterator;
//  typedef typename container_type::const_iterator const_iterator;

  has_many() {}

  iterator insert(iterator pos, const oos::object_ptr<T> &value)
  {
    // create new has_many
    if (indexed_) {

    }
    item_type *item = create_item(value);
    object_ptr<item_type> iptr(item);
    if (this->ostore_) {
      this->ostore_->insert(iptr);
//      ostore_->mark_modified()
    }
    return container_.insert(pos, iptr);
  }

  void push_back(const oos::object_ptr<T> &value)
  {
    insert(end(), value);
  }

  iterator begin() { return iterator(container_.begin()); }
  iterator end() { return iterator(container_.end()); }

//  const_iterator begin() const { return container_.begin(); }
//  const_iterator end() const { return container_.end(); }

  iterator erase(iterator i)
  {

    return iterator();
//    return iterator(container_.erase(i));
  }
  iterator erase(iterator start, iterator end) { return iterator(container_.erase(start, end)); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }

private:
  has_many_item<T>* create_item(const oos::object_ptr<T> &value)
  {
    return new has_many_item<T>(owner_field_, item_field_, base::owner_id_, value);
  }
private:
  container_type container_;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";

  bool indexed_ = false;
};

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::list>::value>::type> : public basic_has_many<T>
{
public:

  typedef basic_has_many<T> base;
  typedef has_many_item<T> value_type;
//  typedef oos::has_one<T> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
//  typedef C<oos::has_one<T>, std::allocator<oos::has_one<T>>> container_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

  explicit has_many() {}

  iterator insert(iterator pos, const oos::object_ptr<T> &value)
  {
    // create new has_many
    value_type *item = create_item(value);
    return container_.insert(pos, item);
  }

  void push_front(const oos::object_ptr<T> &value)
  {
    // create new has_many
    value_type item = create_item(value);
    container_.push_front(item);
  }

  void push_back(const oos::object_ptr<T> &value)
  {
    // create new has_many
    value_type item = create_item(value);
    container_.push_back(item);
  }

  iterator begin() { return container_.begin(); }
  iterator end() { return container_.end(); }

  const_iterator begin() const { return container_.begin(); }
  const_iterator end() const { return container_.end(); }

  iterator erase(iterator i) { return container_.erase(i); }
  iterator erase(iterator start, iterator end) { return container_.erase(start, end); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }

private:
  has_many_item<T>* create_item(const oos::object_ptr<T> &value)
  {
    return new has_many_item<T>(owner_field_, item_field_, base::owner_id_, value);
  }
private:
  container_type container_;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";
};
/*
template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::set>::value>::type> : public basic_has_many<T>
{
public:
  typedef C<oos::has_one<T>, std::less<oos::has_one<T>>, std::allocator<oos::has_one<T>>> container_type;
  typedef container_type& container_reference;

  void insert(T *value) {}

private:
  C<oos::has_one<T>, std::less<oos::has_one<T>>, std::allocator<oos::has_one<T>>> container;
};
*/
}
#endif //OOS_HAS_MANY_HPP
