#ifndef MATADOR_HTTP_EXPORT_HPP
#define MATADOR_HTTP_EXPORT_HPP

#ifdef _MSC_VER
//  #ifdef matador_http_EXPORTS
//    #define OOS_HTTP_API __declspec(dllexport)
//    #define EXPIMP_HTTP_TEMPLATE
//  #else
//    #define OOS_HTTP_API __declspec(dllimport)
//    #define EXPIMP_HTTP_TEMPLATE extern
//  #endif
  #define OOS_HTTP_API
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#endif //MATADOR_HTTP_EXPORT_HPP
