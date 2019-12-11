//
// Created by sascha on 06.03.16.
//

#ifndef OOS_VALUE_SERIALIZER_HPP
#define OOS_VALUE_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
#define OOS_SQL_API __declspec(dllexport)
#define EXPIMP_SQL_TEMPLATE
#else
#define OOS_SQL_API __declspec(dllimport)
#define EXPIMP_SQL_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/memory.hpp"

#include "matador/sql/dialect_token.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API value_serializer : public serializer
{
public:
  value_serializer() = default;
  ~value_serializer() override = default;

  template<class T>
  values *execute(T &x) {
    values_ = matador::make_unique<values>();
    matador::access::serialize(*this, x);
    return values_.release();
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
  void serialize(const char *id, long long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, unsigned long long &x) override;
  void serialize(const char *id, float &x) override;
  void serialize(const char *id, double &x) override;
  void serialize(const char *id, bool &x) override;
  void serialize(const char *id, char *x, size_t s) override;
  void serialize(const char *id, std::string &x, size_t s) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, date &x) override;
  void serialize(const char *id, time &x) override;
  void serialize(const char *id, identifiable_holder &x, cascade_type) override;
  void serialize(const char *id, basic_identifier &x) override;
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

private:
  std::unique_ptr<values> values_;
};

/// @endcond

}
}
#endif //OOS_VALUE_SERIALIZER_HPP
