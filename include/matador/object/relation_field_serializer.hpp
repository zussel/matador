#ifndef MATADOR_RELATION_FIELD_SERIALIZER_HPP
#define MATADOR_RELATION_FIELD_SERIALIZER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"

#include "matador/object/object_holder_type.hpp"

#include <string>

namespace matador {

class object_proxy;
class object_holder;
template < class T, matador::object_holder_type OHT >
class object_pointer;
template < class T >
using object_ptr = object_pointer<T, object_holder_type::OBJECT_PTR>;
class abstract_has_many;

namespace detail {

class relation_field_serializer
{
public:

  template < class T >
  void set(const std::string &field, const object_ptr<T> &obj, object_proxy *value);
  template < class T >
  void clear(const std::string &field, const object_ptr<T> &obj);
  template < class T >
  void append(const std::string &field, const object_ptr<T> &obj, object_proxy *value);
  template < class T >
  void remove(const std::string &field, const object_ptr<T> &obj, object_proxy *value);

  template < class T >
  void serialize(T &x);
  template < class T >
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) {}
  void serialize(const char *, object_holder &x, cascade_type);
  void serialize(const char *, abstract_has_many &x, const char*, const char*);

private:
  enum serialize_action { SET, CLEAR, APPEND, REMOVE };

  template < class T >
  void execute(serialize_action action, const std::string &field, const object_ptr<T> &obj, object_proxy *value);
private:
  object_proxy *value_;
  std::string field_;
  serialize_action action_;
};

template<class T>
void relation_field_serializer::serialize(T &x)
{
  matador::access::serialize(*this, x);
}

}
}

#endif //MATADOR_RELATION_FIELD_SERIALIZER_HPP
