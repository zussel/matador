//
// Created by sascha on 2/25/16.
//

#ifndef OOS_TYPED_COLUMN_SERIALIZER_HPP
#define OOS_TYPED_COLUMN_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
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

#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/identifier.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API typed_column_serializer : public serializer
{
public:
  typed_column_serializer();
  ~typed_column_serializer() override = default;

  template < class T >
  columns* execute(T &x)
  {
    cols_.reset(new columns);
    matador::access::serialize(*this, x);
    return cols_.release();
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, float &x) override;
  void serialize(const char *id, double &x) override;
  void serialize(const char *id, bool &x) override;
  void serialize(const char *id, char *x, size_t s) override;
  void serialize(const char *id, std::string &x, size_t) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, date &x) override;
  void serialize(const char *id, time &x) override;
  void serialize(const char *id, identifiable_holder &x, cascade_type) override;
  void serialize(const char *id, basic_identifier &x) override;

//  template < class V >
//  void serialize(const char *, identifier<V> &) { }
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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
