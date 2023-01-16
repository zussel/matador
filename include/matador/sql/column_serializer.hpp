#ifndef OOS_COLUMN_SERIALIZER_HPP
#define OOS_COLUMN_SERIALIZER_HPP

#include "matador/sql/export.hpp"

#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"

namespace matador {


namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API column_serializer {
public:
  explicit column_serializer(columns::t_brackets brackets);

  template<class T>
  columns *execute(T &x) {
    cols_ = std::make_unique<columns>(brackets_);
    matador::access::serialize(*this, x);
    return cols_.release();
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_attribute(const char *id, char &x);
  void on_attribute(const char *id, short &x);
  void on_attribute(const char *id, int &x);
  void on_attribute(const char *id, long &x);
  void on_attribute(const char *id, long long &x);
  void on_attribute(const char *id, unsigned char &x);
  void on_attribute(const char *id, unsigned short &x);
  void on_attribute(const char *id, unsigned int &x);
  void on_attribute(const char *id, unsigned long &x);
  void on_attribute(const char *id, unsigned long long &x);
  void on_attribute(const char *id, float &x);
  void on_attribute(const char *id, double &x);
  void on_attribute(const char *id, bool &x);
  void on_attribute(const char *id, char *x, size_t s);
  void on_attribute(const char *id, std::string &x, size_t s);
  void on_attribute(const char *id, std::string &x);
  void on_attribute(const char *id, date &x);
  void on_attribute(const char *id, time &x);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  void on_primary_key(const char *id, basic_identifier &x);
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  columns::t_brackets brackets_;
  std::unique_ptr<columns> cols_;
};

/// @endcond

}
}

#endif //OOS_COLUMN_SERIALIZER_HPP
