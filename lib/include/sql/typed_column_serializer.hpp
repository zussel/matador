//
// Created by sascha on 2/25/16.
//

#ifndef OOS_TYPED_COLUMN_SERIALIZER_HPP
#define OOS_TYPED_COLUMN_SERIALIZER_HPP

#include "sql/sql.hpp"
#include "sql/column.hpp"

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"
#include "tools/serializer.hpp"

namespace oos {
namespace detail {

class typed_column_serializer : public serializer
{
public:
  explicit typed_column_serializer(sql &d);
  ~typed_column_serializer() {}

  template < class T >
  columns* serialize(T &x)
  {
    cols_.reset(new columns);
    oos::access::serialize(*this, x);
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
  std::unique_ptr<detail::columns> cols_;
  size_t index_ = 0;
  sql &sql_;
  bool generate_pk_column_ = false;
};

}

}

#endif //OOS_TYPED_COLUMN_SERIALIZER_HPP
