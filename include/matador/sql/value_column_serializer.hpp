//
// Created by sascha on 03.04.16.
//

#ifndef OOS_VALUE_COLUMN_SERIALIZER_HPP
#define OOS_VALUE_COLUMN_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef oos_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/column.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond OOS_DEV

class OOS_SQL_API value_column_serializer : public serializer {
public:
  value_column_serializer();

  template<class T>
  void append_to(const std::shared_ptr<columns> cols, T &x)
  {
    cols_ = cols;
    matador::access::serialize(static_cast<serializer&>(*this), x);
  }

  void serialize(const char *id, char &x);
  void serialize(const char *id, short &x);
  void serialize(const char *id, int &x);
  void serialize(const char *id, long &x);
  void serialize(const char *id, unsigned char &x);
  void serialize(const char *id, unsigned short &x);
  void serialize(const char *id, unsigned int &x);
  void serialize(const char *id, unsigned long &x);
  void serialize(const char *id, float &x);
  void serialize(const char *id, double &x);
  void serialize(const char *id, bool &x);
  void serialize(const char *id, char *x, size_t s);
  void serialize(const char *id, varchar_base &x);
  void serialize(const char *id, std::string &x);
  void serialize(const char *id, date &x);
  void serialize(const char *id, time &x);
  void serialize(const char *id, identifiable_holder &x, cascade_type);
  void serialize(const char *id, basic_identifier &x);

private:
  std::shared_ptr<columns> cols_;
};

/// @endcond

}
}
#endif //OOS_VALUE_COLUMN_SERIALIZER_HPP
