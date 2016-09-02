//
// Created by sascha on 9/2/16.
//

#ifndef OOS_OBJECT_PROXY_ACCESSOR_HPP
#define OOS_OBJECT_PROXY_ACCESSOR_HPP

namespace oos {

class object_proxy;
class object_holder;

namespace detail {

class object_proxy_accessor
{
public:
  oos::object_proxy* proxy(const object_holder &holder) const;
};

}
}
#endif //OOS_OBJECT_PROXY_ACCESSOR_HPP
