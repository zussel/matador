//
// Created by sascha on 04.03.16.
//

#ifndef OOS_COLUMN_SERIALIZER_HPP
#define OOS_COLUMN_SERIALIZER_HPP

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

namespace matador {

class varchar_base;

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API column_serializer : public serializer {
public:
  explicit column_serializer(columns::t_brackets brackets);

  ~column_serializer()  override = default;

  template<class T>
  columns *execute(T &x) {
    cols_ = std::make_unique<columns>(brackets_);
    matador::access::serialize(static_cast<serializer&>(*this), x);
    return cols_.release();
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
  void serialize(const char *id, varchar_base &x) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, date &x) override;
  void serialize(const char *id, time &x) override;
  void serialize(const char *id, identifiable_holder &x, cascade_type) override;
  void serialize(const char *id, basic_identifier &x) override;

private:
  columns::t_brackets brackets_;
  std::unique_ptr<columns> cols_;
};

/// @endcond

}
}

#endif //OOS_COLUMN_SERIALIZER_HPP
