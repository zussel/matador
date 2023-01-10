#ifndef MATADOR_OBJECT_EXPORT_HPP
#define MATADOR_OBJECT_EXPORT_HPP

#ifdef _MSC_VER
//  #ifdef matador_object_EXPORTS
//    #define MATADOR_OBJECT_API __declspec(dllexport)
//    #define EXPIMP_OBJECT_TEMPLATE
//  #else
//    #define MATADOR_OBJECT_API __declspec(dllimport)
//    #define EXPIMP_OBJECT_TEMPLATE extern
//  #endif
  #define MATADOR_OBJECT_API
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#else
  #define MATADOR_OBJECT_API
#endif

#endif //MATADOR_OBJECT_EXPORT_HPP
