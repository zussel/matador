//
// Created by sascha on 04.03.16.
//

#ifndef OOS_COLUMN_SERIALIZER_HPP
#define OOS_COLUMN_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "oos/sql/sql.hpp"
#include "oos/sql/column.hpp"

#include "oos/utils/access.hpp"
#include "oos/utils/cascade_type.hpp"
#include "oos/utils/serializer.hpp"

namespace oos {

class varchar_base;

namespace detail {

/// @cond OOS_DEV

class OOS_API column_serializer : public serializer {
public:
  explicit column_serializer(columns::t_brackets brackets);

  virtual ~column_serializer() { }

  template<class T>
  columns *execute(T &x) {
    cols_.reset(new columns(brackets_));
    oos::access::serialize(static_cast<serializer&>(*this), x);
    return cols_.release();
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
  columns::t_brackets brackets_;
  std::unique_ptr<columns> cols_;
};

/// @endcond

}
}

#endif //OOS_COLUMN_SERIALIZER_HPP
