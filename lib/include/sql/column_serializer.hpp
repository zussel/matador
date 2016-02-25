//
// Created by sascha on 2/25/16.
//

#ifndef OOS_COLUMN_SERIALIZER_HPP
#define OOS_COLUMN_SERIALIZER_HPP

#include "sql/sql.hpp"

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"
#include "tools/identifier.hpp"

namespace oos {
namespace detail {

class column_serializer : public serializer
{
public:
  explicit column_serializer(sql &d);
  ~column_serializer() {}

  template < class T >
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
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
  size_t index_ = 0;
  sql &sql_;
};

}

}

#endif //OOS_COLUMN_SERIALIZER_HPP
