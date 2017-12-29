//
// Created by sascha on 9/2/16.
//

#ifndef OOS_OBJECT_PROXY_ACCESSOR_HPP
#define OOS_OBJECT_PROXY_ACCESSOR_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define MATADOR_OBJECT_API
#endif

namespace matador {

class object_proxy;
class object_holder;

namespace detail {

/// @cond MATADOR_DEV

class MATADOR_OBJECT_API object_proxy_accessor
{
public:
  matador::object_proxy* proxy(const object_holder &holder) const;
};

/// @endcond

}
}
#endif //OOS_OBJECT_PROXY_ACCESSOR_HPP
