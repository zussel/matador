//
// Created by sascha on 9/29/17.
//

#ifndef MATADOR_RELATION_DATA_HPP
#define MATADOR_RELATION_DATA_HPP

#include "matador/utils/basic_identifier.hpp"

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_holder_type.hpp"
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

template < class T >
class relation_data : public basic_relation_data
{
public:
  relation_data() : tindex_(std::type_index(typeid(T))) {}

  void append_data(const identifier_ptr &id, const T &data)
  {
    id_multi_map_.insert(std::make_pair(id, data));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier_ptr &id, basic_has_many<T, C> &container)
  {
    std::cout << "try to insert for pk " << *id << " into container (size: " << container.size() << ")\n";
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_map<identifier_ptr, T> id_multi_map_;
};

template < class T >
class relation_data<object_pointer<T, object_holder_type::OBJECT_PTR>> : public basic_relation_data
{
public:
  typedef object_pointer<T, object_holder_type::OBJECT_PTR> value_type;

  relation_data() : tindex_(std::type_index(typeid(T))) {}

  void append_data(const identifier_ptr &id, const value_type &data)
  {
    id_multi_map_.insert(std::make_pair(id, data));
  }

  template < template <class ...> class C >
  void insert_into_container(const identifier_ptr &id, basic_has_many<T, C> &container)
  {
    std::cout << "try to insert for pk " << *id << " into container (size: " << container.size() << ")\n";
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_map<identifier_ptr, value_type> id_multi_map_;
};

}
}
#endif //MATADOR_RELATION_DATA_HPP
