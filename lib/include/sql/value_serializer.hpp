//
// Created by sascha on 06.03.16.
//

#ifndef OOS_VALUE_SERIALIZER_HPP
#define OOS_VALUE_SERIALIZER_HPP

#include "tools/access.hpp"
#include "tools/serializer.hpp"
#include "dialect_token.hpp"

namespace oos {

class sql;

namespace detail {

class value_serializer : public serializer
{
public:
  value_serializer();
  virtual ~value_serializer() { }

  template<class T>
  values *execute(T &x) {
    values_.reset(new values);
    oos::access::serialize(static_cast<serializer&>(*this), x);
    return values_.release();
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
  std::unique_ptr<values> values_;
};

}
}
#endif //OOS_VALUE_SERIALIZER_HPP
