//
// Created by sascha on 9/2/16.
//

#ifndef OOS_OBJECT_PROXY_ACCESSOR_HPP
#define OOS_OBJECT_PROXY_ACCESSOR_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

namespace oos {

class object_proxy;
class object_holder;

namespace detail {

class OOS_API object_proxy_accessor
{
public:
  oos::object_proxy* proxy(const object_holder &holder) const;
};

}
}
#endif //OOS_OBJECT_PROXY_ACCESSOR_HPP
