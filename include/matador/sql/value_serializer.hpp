#ifndef OOS_VALUE_SERIALIZER_HPP
#define OOS_VALUE_SERIALIZER_HPP

#include "matador/sql/export.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/dialect_token.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API value_serializer
{
public:
  value_serializer() = default;
  ~value_serializer() = default;

  template<class T>
  values *execute(T &x) {
    values_ = std::make_unique<values>();
    matador::access::serialize(*this, x);
    return values_.release();
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_attribute(const char *id, char &x, long /*size*/ = -1);
  void on_attribute(const char *id, short &x, long /*size*/ = -1);
  void on_attribute(const char *id, int &x, long /*size*/ = -1);
  void on_attribute(const char *id, long &x, long /*size*/ = -1);
  void on_attribute(const char *id, long long &x, long /*size*/ = -1);
  void on_attribute(const char *id, unsigned char &x, long /*size*/ = -1);
  void on_attribute(const char *id, unsigned short &x, long /*size*/ = -1);
  void on_attribute(const char *id, unsigned int &x, long /*size*/ = -1);
  void on_attribute(const char *id, unsigned long &x, long /*size*/ = -1);
  void on_attribute(const char *id, unsigned long long &x, long /*size*/ = -1);
  void on_attribute(const char *id, float &x, long /*size*/ = -1);
  void on_attribute(const char *id, double &x, long /*size*/ = -1);
  void on_attribute(const char *id, bool &x, long /*size*/ = -1);
  void on_attribute(const char *id, char *x, long /*size*/ = -1);
  void on_attribute(const char *id, std::string &x, long /*size*/ = -1);
  void on_attribute(const char *id, date &x, long /*size*/ = -1);
  void on_attribute(const char *id, time &x, long /*size*/ = -1);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  void on_primary_key(const char *id, basic_identifier &x, long /*size*/ = -1);
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  std::unique_ptr<values> values_;
};

/// @endcond

}
}
#endif //OOS_VALUE_SERIALIZER_HPP
