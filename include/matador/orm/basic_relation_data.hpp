//
// Created by sascha on 9/29/17.
//

#ifndef MATADOR_RELATION_DATA_HPP
#define MATADOR_RELATION_DATA_HPP

#include "matador/utils/basic_identifier.hpp"

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_holder_type.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/identifier_proxy_map.hpp"

#include <memory>


namespace matador {

/// @cond MATADOR_DEV

template < class T, object_holder_type OHT >
class object_pointer;

namespace detail {

class basic_relation_data : public object_proxy_accessor
{
public:
  virtual ~basic_relation_data() = default;

  virtual const std::type_index& type_index() const = 0;

  void increment_reference_count(object_proxy &proxy)
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

  relation_data() : tindex_(std::type_index(typeid(value_type))) {}

  void append_data(basic_identifier *id, const value_type &data, object_proxy *owner)
  {
    id_multi_map_.insert(std::make_pair(id, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(basic_identifier *id, basic_has_many<value_type, C> &container)
  {
    auto range = id_multi_map_.equal_range(id);
    for (auto i = range.first; i != range.second; ++i)
    {
      container.append(has_many_item_holder<value_type>(i->second.first, i->second.second));
    }
    id_multi_map_.erase(id);
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_multimap<basic_identifier*, std::pair<value_type, object_proxy*>, identifier_hash<basic_identifier>, identifier_equal> id_multi_map_;
};

template < class T >
class relation_data<T, typename std::enable_if<!matador::is_builtin<T>::value>::type> : public basic_relation_data
{
public:
  typedef typename T::object_type value_type;

  relation_data() : tindex_(std::type_index(typeid(value_type))) {}

  void append_data(basic_identifier *id, const T &data, object_proxy *owner)
  {
    id_multi_map_.insert(std::make_pair(id, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(basic_identifier *id, basic_has_many<value_type, C> &container)
  {
    auto range = id_multi_map_.equal_range(id);
    for (auto i = range.first; i != range.second; ++i)
    {
      container.append(has_many_item_holder<value_type>(i->second.first, i->second.second));
      if (!std::is_base_of<basic_has_many_item, value_type>::value && i->second.second == nullptr) {
        ++(*proxy(i->second.first));
      }

    }
    id_multi_map_.erase(id);
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_multimap<basic_identifier*, std::pair<T, object_proxy*>, identifier_hash<basic_identifier>, identifier_equal> id_multi_map_;
};

/// @endcond

}
}
#endif //MATADOR_RELATION_DATA_HPP
