//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_MANY_HPP
#define OOS_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/container_type_traits.hpp"

#include <vector>
#include <list>

namespace oos {

template < class T, class O >
class basic_has_many
{
public:
  typedef has_one<T> value_type;
  typedef O owner_type;
  typedef value_type& reference;
  typedef value_type* pointer;

protected:
  friend class object_inserter;

  object_store *ostore_ = nullptr;

  owner_type *owner_ = nullptr;
};

template < class T, class O, template <class ...> class C = std::vector, class Enable = void >
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

  has_many_item()
    : owner_id_("owner_id")
    , item_id_("item_id")
  { }

  has_many_item(const std::string &owner_id, const std::string &item_id)
    : owner_id_(owner_id)
    , item_id_(item_id)
  { }

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(owner_id_.c_str(), owner_);
    serializer.serialize(item_id_.c_str(), item_);
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
  basic_identifier owner_;
  has_one<T> item_;

  std::string owner_id_;
  std::string item_id_;
};

template < class T, class O, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value || is_same_container_type<C, std::list>::value>::type> : public basic_has_many<T>
{
public:

  typedef has_many_item<T> value_type;
//  typedef oos::has_one<T> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
//  typedef C<oos::has_one<T>, std::allocator<oos::has_one<T>>> container_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

  void push_back(const oos::object_ptr<T> &value)
  {
//    container_.push_back(value);
  }

  iterator begin() { return container_.begin(); }
  iterator end() { return container_.end(); }

  const_iterator begin() const { return container_.begin(); }
  const_iterator end() const { return container_.end(); }

  iterator erase(iterator i) { return container_.erase(i); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }
private:
  container_type container_;
};

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

}
#endif //OOS_HAS_MANY_HPP
