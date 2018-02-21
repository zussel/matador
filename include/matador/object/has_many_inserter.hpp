//
// Created by sascha on 02.05.17.
//

#ifndef MATADOR_HAS_MANY_INSERTER_HPP
#define MATADOR_HAS_MANY_INSERTER_HPP

#include "matador/object/basic_has_many.hpp"
#include "matador/object/container_type_traits.hpp"

#include "matador/utils/is_builtin.hpp"

#include <vector>
#include <list>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

template<class T, template < class... > class C, class Enable >
class has_many_inserter;

template<class T, template < class... > class C>
class has_many_inserter<T, C, typename std::enable_if<
  !is_builtin<T>::value &&
  (matador::is_same_container_type<C, std::list>::value || matador::is_same_container_type<C, std::vector>::value)>::type>
{
public:
  typedef typename basic_has_many<T, C>::iterator iterator;
  typedef typename has_many_iterator_traits<T, C>::holder_type holder_type;
  typedef has_many<T, C> container_type;

  has_many_inserter(container_type &container) : container_(container) {}

  void insert(const holder_type &/*holder*/)
  {
    if (container_.relation_info_ != nullptr) {

      if (container_.relation_info_->type == detail::basic_relation_endpoint::BELONGS_TO) {
        /*
         * foreign end of relation is
         * belongs_to, set owner into
         * corresponding field
         */
      } else if (container_.relation_info_->type == detail::basic_relation_endpoint::HAS_MANY) {
        /*
         * foreign end of relation is
         * also has_many, insert has_many_to_many_item
         * append it to foreign relation
         */
      }
    } else {
      std::cout << "what should I do here?\n";
    }
  }

  void append_proxy(object_proxy *) { }

  container_type &container_;
};

template<class T, template < class... > class C>
class has_many_inserter<T, C, typename std::enable_if<
  is_builtin<T>::value &&
  (matador::is_same_container_type<C,std::list>::value || matador::is_same_container_type<C, std::vector>::value)>::type>
{
public:
  typedef typename basic_has_many<T, C>::iterator iterator;
  typedef typename has_many_iterator_traits<T, C>::holder_type holder_type;
  typedef typename basic_has_many<T, C>::mark_modified_owner_func mark_modified_owner_func;
  typedef has_many<T, C> container_type;

  has_many_inserter(container_type &container) : container_(container) {}

  void insert(const holder_type &) { }

  void insert(iterator) { }

  void append_proxy(object_proxy*) {}

  container_type &container_;
};

/// @endcond

}
}
#endif //MATADOR_HAS_MANY_INSERTER_HPP
