//
// Created by sascha on 2/25/16.
//

#ifndef OOS_TYPED_COLUMN_SERIALIZER_HPP
#define OOS_TYPED_COLUMN_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef oos_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "oos/sql/sql.hpp"
#include "oos/sql/column.hpp"

#include "oos/utils/access.hpp"
#include "oos/utils/cascade_type.hpp"
#include "oos/utils/serializer.hpp"
#include "oos/utils/identifier.hpp"

namespace oos {
namespace detail {

/// @cond OOS_DEV

class OOS_SQL_API typed_column_serializer : public serializer
{
public:
  typed_column_serializer();
  virtual ~typed_column_serializer() {}

  template < class T >
  columns* execute(T &x)
  {
    cols_.reset(new columns);
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

  template < class V >
  void serialize(const char *id, identifier<V> &)
  {
    std::cout << "serializing " << id << " identifier of type " << typeid(V).name() << '\n';
  }

private:
  std::unique_ptr<columns> cols_;
  size_t index_ = 0;
  typedef std::function<std::shared_ptr<column> (const char*, data_type, size_t)> t_create_column_func;
  t_create_column_func create_column_func_;
  typedef std::function<std::shared_ptr<column> (const char*, size_t, data_type, size_t)> t_create_varchar_column_func;
  t_create_varchar_column_func create_varchar_column_func_;
};

/// @endcond

}

}

#endif //OOS_TYPED_COLUMN_SERIALIZER_HPP
