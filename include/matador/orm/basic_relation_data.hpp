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

template < class T, object_holder_type OHT >
class object_pointer;

namespace detail {

class basic_relation_data
{
public:
  virtual ~basic_relation_data() = default;

  virtual const std::type_index& type_index() const = 0;
};

template < class T, class Enabled = void >
class relation_data;

template < class T >
class relation_data<T, typename std::enable_if<matador::is_builtin<T>::value>::type> : public basic_relation_data
{
public:
  relation_data() : tindex_(std::type_index(typeid(T))) {}

  void append_data(const identifier_ptr &id, const T &data, object_proxy *owner)
  {
//    std::cout << "insert value " << data << " for pk " << *id << " (type: " << typeid(T).name() << ")\n";
    id_multi_map_.insert(std::make_pair(id, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier_ptr &id, basic_has_many<T, C> &container)
  {
//    std::cout << "try to insert for pk " << *id << " into container (size: " << container.size() << ", type: " << typeid(T).name() << ")\n";
    auto range = id_multi_map_.equal_range(id);
    for (auto i = range.first; i != range.second; ++i)
    {
//      std::cout << "insert into container " << &i->second << " (type: " << typeid(T).name() << ")\n";
      container.append(has_many_item_holder<T>(i->second.first, i->second.second));
    }
    id_multi_map_.erase(id);
//    std::cout << "inserted for pk " << *id << " into container (size: " << container.size() << ", type: " << typeid(T).name() << ")\n";
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_multimap<identifier_ptr, std::pair<T, object_proxy*>, identifier_hash<identifier_ptr>, identifier_equal> id_multi_map_;
};

template < class T >
class relation_data<T, typename std::enable_if<!matador::is_builtin<T>::value>::type> : public basic_relation_data
{
public:
//  typedef object_pointer<T, object_holder_type::OBJECT_PTR> value_type;
  typedef typename T::object_type value_type;

  relation_data() : tindex_(std::type_index(typeid(value_type))) {}

  void append_data(const identifier_ptr &id, const T &data, object_proxy *owner)
  {
//    std::cout << "insert value " << data.id() << " for pk " << *id << " (type: " << typeid(T).name() << ")\n";
    id_multi_map_.insert(std::make_pair(id, std::make_pair(data, owner)));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier_ptr &id, basic_has_many<value_type, C> &container)
  {
//    std::cout << "try to insert for pk " << *id << " into container (size: " << container.size() << ", type: " << typeid(T).name() << ")\n";
    auto range = id_multi_map_.equal_range(id);
    for (auto i = range.first; i != range.second; ++i)
    {
//      std::cout << "insert into container " << &i->second << " (type: " << typeid(T).name() << ")\n";
      container.append(has_many_item_holder<value_type>(i->second.first, i->second.second));
    }
    id_multi_map_.erase(id);
//    std::cout << "inserted for pk " << *id << " into container (size: " << container.size() << ", type: " << typeid(T).name() << ")\n";
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_multimap<identifier_ptr, std::pair<T, object_proxy*>, identifier_hash<identifier_ptr>, identifier_equal> id_multi_map_;
};

}
}
#endif //MATADOR_RELATION_DATA_HPP
