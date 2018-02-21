//
// Created by sascha on 04.05.17.
//

#ifndef MATADOR_HAS_MANY_DELETER_HPP
#define MATADOR_HAS_MANY_DELETER_HPP

#include "matador/object/basic_has_many.hpp"
#include "matador/object/container_type_traits.hpp"

#include "matador/utils/is_builtin.hpp"

#include <vector>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

template<class T, template<class...> class C, class Enable>
class has_many_deleter;

template<class T, template < class... > class C>
class has_many_deleter<T, C, typename std::enable_if<
  !is_builtin<T>::value &&
  (matador::is_same_container_type<C,std::list>::value || matador::is_same_container_type<C, std::vector>::value)>::type>
{
public:
  typedef typename basic_has_many<T, C>::iterator iterator;
  typedef typename has_many_iterator_traits<T, C>::value_type value_type;
  typedef has_many<T, C> container_type;

  has_many_deleter(container_type &container) : container_(container) {}

  void remove(iterator /*i*/)
  {
    if (container_.relation_info_ != nullptr) {
      if (container_.relation_info_->type == detail::basic_relation_endpoint::BELONGS_TO) {
        /*
         * foreign end of relation is
         * belongs_to, clear owner from
         * corresponding field
         */
//        container_.relation_info_->clear(*container_.store(), *i);
      } else if (container_.relation_info_->type == detail::basic_relation_endpoint::HAS_MANY) {
        /*
         * foreign end of relation is
         * also has_many, insert has_many_to_many_item
         */
//        container_.relation_info_->remove(*container_.store(), *i, container_.owner_);
//        container_.store()->remove(rtype);
      }
    } else {
//      container_.store()->remove(rtype);
    }
  }

  void remove_proxy(object_proxy *proxy)
  {
    auto val = value_type(proxy);
    container_.remove(val);
  }

  container_type &container_;
};

template<class T, template < class... > class C>
class has_many_deleter<T, C, typename std::enable_if<
  is_builtin<T>::value &&
  (matador::is_same_container_type<C,std::list>::value || matador::is_same_container_type<C, std::vector>::value)>::type>
{
public:
  typedef typename basic_has_many<T, C>::iterator iterator;
  typedef typename has_many_iterator_traits<T, C>::value_type value_type;
  typedef has_many<T, C> container_type;

  has_many_deleter(container_type &container) : container_(container) {}

  void remove(iterator /*i*/)
  {
//    container_.relation_info_->remove(*container_.store(), *i, container_.owner_);
//    relation_type rtype(*i.iter_);
//    container_.store()->remove(rtype);
  }

  void remove_proxy(object_proxy*) {}

  container_type &container_;
};

/// @endcond

}
}

#endif //MATADOR_HAS_MANY_DELETER_HPP
