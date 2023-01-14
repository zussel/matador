#ifndef MATADOR_UTILS_EXPORT_HPP
#define MATADOR_UTILS_EXPORT_HPP

#ifdef _MSC_VER
#define OOS_UTILS_API
//  #ifdef matador_utils_EXPORTS
//    #define OOS_UTILS_API __declspec(dllexport)
//    #define EXPIMP_UTILS_TEMPLATE
//  #else
//    #define OOS_UTILS_API __declspec(dllimport)
//    #define EXPIMP_UTILS_TEMPLATE extern
//  #endif
  #define OOS_UTILS_API
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#endif //MATADOR_UTILS_EXPORT_HPP