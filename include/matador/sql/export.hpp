#ifndef MATADOR_SQL_EXPORT_HPP
#define MATADOR_SQL_EXPORT_HPP

#ifdef _MSC_VER
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #define OOS_SQL_API
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
  #define OOS_SQL_API
#endif

#endif //MATADOR_SQL_EXPORT_HPP
