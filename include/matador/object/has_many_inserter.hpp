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

  void insert(const holder_type &holder)
  {
    if (container_.relation_info_ != nullptr) {
      if (container_.relation_info_->type == detail::relation_field_endpoint::BELONGS_TO) {
        /*
         * foreign end of relation is
         * belongs_to, set owner into
         * corresponding field
         */
        container_.relation_info_->set(*container_.store(), holder.value(), container_.owner_);
      } else if (container_.relation_info_->type == detail::relation_field_endpoint::HAS_MANY) {
        /*
         * foreign end of relation is
         * also has_many, insert has_many_to_many_item
         * append it to foreign relation
         */
//        container_.relation_info_->insert(*container_.store(), holder.value(), container_.owner_);
        container_.relation_info_->append(*container_.store(), holder.value(), container_.owner_);
//        container_.store()->insert(rtype);
      }
    } else {
      std::cout << "what should I do here?\n";
//      container_.store()->insert(rtype);
    }
    container_.mark_modified_owner_(*container_.store(), container_.owner_);
  }

  void append_proxy(object_proxy */*proxy*/)
  {
//    item_type *item = new item_type(
//      container_.owner_field(), container_.item_field(), container_.owner_id_, proxy
//    );
//    relation_type iptr(item);
//    iterator i(container_.container_.insert(container_.container_.end(), iptr));
//    if (container_.store()) {
//      insert(i);
//    }
  }

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

  void insert(const holder_type &holder)
  {
    container_.relation_info_->insert(*container_.store(), holder.value(), container_.owner_);

  }

  void insert(iterator i)
  {

//    relation_type rtype(*i.iter_);
//    container_.store()->insert(rtype);
//    container_ .mark_modified_owner_(*container_.store(), container_.owner_);
  }

  void append_proxy(object_proxy*) {}

  container_type &container_;
};

}
}
#endif //MATADOR_HAS_MANY_INSERTER_HPP
