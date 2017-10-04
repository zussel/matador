//
// Created by sascha on 9/29/17.
//

#ifndef MATADOR_RELATION_DATA_HPP
#define MATADOR_RELATION_DATA_HPP

#include "matador/utils/basic_identifier.hpp"
#include "matador/object/object_proxy.hpp"

#include <memory>


namespace matador {

namespace detail {

class relation_data
{
  virtual ~relation_data() = default;

  virtual void insert(std::shared_ptr<basic_identifier> id, object_proxy *relation_item, object_proxy *value) = 0;
};

template < class T >
class relation_has_many_data : public relation_data
{
  void insert(std::shared_ptr<basic_identifier> id, object_proxy *relation_item, object_proxy *value) override
  {

  }
};

template < class T, template <class ...> class C >
class relation_has_one_data : public relation_data
{
  typedef std::unordered_multimap<identifier_ptr, std::pair<object_proxy*, object_proxy*>, identifier_hash<identifier_ptr>, identifier_equal> t_id_rel_multimap;

  void insert(std::shared_ptr<basic_identifier> id, object_proxy *relation_item, object_proxy *value) override
  {

  }

  t_id_rel_multimap idrel_map_;
};

}
}
#endif //MATADOR_RELATION_DATA_HPP
