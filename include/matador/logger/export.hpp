#ifndef MATADOR_LOGGER_EXPORT_HPP
#define MATADOR_LOGGER_EXPORT_HPP

#ifdef _MSC_VER
//#ifdef matador_logger_EXPORTS
//    #define OOS_LOGGER_API __declspec(dllexport)
//    #define EXPIMP_LOGGER_TEMPLATE
//  #else
//    #define OOS_LOGGER_API __declspec(dllimport)
//    #define EXPIMP_LOGGER_TEMPLATE extern
//  #endif
  #define OOS_LOGGER_API
  #pragma warning(disable: 4251)
#else
#define OOS_LOGGER_API
#endif

#endif //MATADOR_LOGGER_EXPORT_HPP
