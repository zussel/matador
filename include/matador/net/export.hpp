#ifndef MATADOR_NET_EXPORT_HPP
#define MATADOR_NET_EXPORT_HPP

#ifdef _MSC_VER
//  #ifdef matador_net_EXPORTS
//    #define OOS_NET_API __declspec(dllexport)
//    #define EXPIMP_NET_TEMPLATE
//  #else
//    #define OOS_NET_API __declspec(dllimport)
//    #define EXPIMP_NET_TEMPLATE extern
//  #endif
  #define OOS_NET_API
  #pragma warning(disable: 4251)
#else
  #define OOS_NET_API
#endif

#endif //MATADOR_NET_EXPORT_HPP