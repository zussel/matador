#ifndef MATADOR_ORM_EXPORT_HPP
#define MATADOR_ORM_EXPORT_HPP

#ifdef _MSC_VER
//#ifdef matador_orm_EXPORTS
//#define OOS_ORM_API __declspec(dllexport)
//#define EXPIMP_ORM_TEMPLATE
//#else
//#define OOS_ORM_API __declspec(dllimport)
//#define EXPIMP_ORM_TEMPLATE extern
//#endif
#define OOS_ORM_API
#pragma warning(disable: 4251)
#else
#define OOS_ORM_API
#endif

#endif //MATADOR_ORM_EXPORT_HPP
