#ifndef MATADOR_JSON_EXPORT_HPP
#define MATADOR_JSON_EXPORT_HPP

#ifdef _MSC_VER
//#ifdef matador_json_EXPORTS
//    #define OOS_JSON_API __declspec(dllexport)
//    #define EXPIMP_JSON_TEMPLATE
//  #else
//    #define OOS_JSON_API __declspec(dllimport)
//    #define EXPIMP_JSON_TEMPLATE extern
//  #endif
  #define OOS_JSON_API
  #pragma warning(disable: 4251)
#else
#define OOS_JSON_API
#endif

#endif //MATADOR_JSON_EXPORT_HPP
