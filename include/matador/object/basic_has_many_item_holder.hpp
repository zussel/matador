//
// Created by sascha on 26.08.17.
//

#ifndef MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP

namespace matador {

class basic_has_many_item_holder
{
public:
  basic_has_many_item_holder() = default;
  ~basic_has_many_item_holder() = default;

  basic_has_many_item_holder& operator=(const basic_has_many_item_holder &x) = default;
  basic_has_many_item_holder& operator=(basic_has_many_item_holder &&x) = default;
  basic_has_many_item_holder(const basic_has_many_item_holder &x) = default;
  basic_has_many_item_holder(basic_has_many_item_holder &&x) = default;
};

}

#endif //MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
