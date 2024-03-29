#ifndef MATADOR_RELATION_DATA_HPP
#define MATADOR_RELATION_DATA_HPP

#include "matador/object/object_proxy.hpp"
#include "matador/object/container.hpp"
#include "matador/object/container_item_holder.hpp"
#include "matador/object/identifier_proxy_map.hpp"

#include <memory>

namespace matador {

/// @cond MATADOR_DEV

template < class T >
class object_ptr;

namespace detail {

class basic_relation_data : public object_proxy_accessor
{
public:
  virtual ~basic_relation_data() = default;

  virtual const std::type_index& type_index() const = 0;

  static void increment_reference_count(object_proxy &proxy)
  {
    ++proxy;
  }
};

template < class T, class Enabled = void >
class relation_data;

template < class T >
class relation_data<T, typename std::enable_if<matador::is_builtin<T>::value>::type> : public basic_relation_data
{
public:
  typedef T value_type;

  relation_data() : type_index_(std::type_index(typeid(value_type))) {}

  void append_data(const identifier &pk, const value_type &data, object_proxy *owner)
  {
    id_multi_map_.insert(std::make_pair(pk, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier &pk, container<value_type, C> &container)
  {
    auto range = id_multi_map_.equal_range(pk);
    for (auto i = range.first; i != range.second; ++i)
    {
      container.append(container_item_holder<value_type>(i->second.first, i->second.second));
    }
    id_multi_map_.erase(pk);
  }

  const std::type_index& type_index() const override
  {
    return type_index_;
  }

private:
  std::type_index type_index_;

  std::unordered_multimap<identifier, std::pair<value_type, object_proxy*>, id_pk_hash, std::equal_to<identifier>> id_multi_map_;
};

template < class T >
class relation_data<T, typename std::enable_if<!matador::is_builtin<T>::value>::type> : public basic_relation_data
{
public:
  typedef typename T::object_type value_type;

  relation_data() : type_index_(std::type_index(typeid(value_type))) {}

  void append_data(const identifier &pk, const T &data, object_proxy *owner)
  {
    id_multi_map_.insert(std::make_pair(pk, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier &pk, container<value_type, C> &container)
  {
    auto range = id_multi_map_.equal_range(pk);
    for (auto i = range.first; i != range.second; ++i)
    {
      container.append(container_item_holder<value_type>(i->second.first, i->second.second));
      if (!std::is_base_of<basic_has_many_to_many_item, value_type>::value && i->second.second == nullptr) {
        ++(*proxy(i->second.first));
      }

    }

    id_multi_map_.erase(pk);
  }

  const std::type_index& type_index() const override
  {
    return type_index_;
  }

private:
  std::type_index type_index_;

  std::unordered_multimap<identifier, std::pair<T, object_proxy*>, id_pk_hash, std::equal_to<identifier>> id_multi_map_;
};

/// @endcond

}
}
#endif //MATADOR_RELATION_DATA_HPP
