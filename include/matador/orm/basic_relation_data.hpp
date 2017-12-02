//
// Created by sascha on 9/29/17.
//

#ifndef MATADOR_RELATION_DATA_HPP
#define MATADOR_RELATION_DATA_HPP

#include "matador/utils/basic_identifier.hpp"

#include "matador/object/object_proxy.hpp"
#include "matador/object/identifier_proxy_map.hpp"

#include <memory>


namespace matador {

namespace detail {

class basic_relation_data
{
public:
  virtual ~basic_relation_data() = default;

  virtual void insert(std::shared_ptr<basic_identifier> id, object_proxy *relation_item, object_proxy *value) = 0;

  virtual const std::type_index& type_index() const = 0;
};

template < class T >
class relation_data : public basic_relation_data
{
public:
  relation_data() : tindex_(std::type_index(typeid(T))) {}

  void insert(std::shared_ptr<basic_identifier> id, object_proxy *relation_item, object_proxy *value) override
  {

  }

  void append_data(const identifier_ptr &id, const T &data)
  {

  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  t_identifier_multimap id_multi_map_;
};

template <>
class relation_data<object_proxy> : public basic_relation_data
{
public:
  relation_data() : tindex_(std::type_index(typeid(object_proxy))) {}

  void insert(std::shared_ptr<basic_identifier> , object_proxy */*owner*/, object_proxy */*value*/) override
  {

  }

  void append_data(const identifier_ptr &id, object_proxy *data)
  {
    id_multi_map_.insert(std::make_pair(id, data));
  }

  const std::type_index& type_index() const override
  {
    return tindex_;
  }

private:
  std::type_index tindex_;

  std::unordered_map<identifier_ptr, object_proxy*> id_multi_map_;
};

}
}
#endif //MATADOR_RELATION_DATA_HPP
