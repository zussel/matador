#ifndef MATADOR_RELATION_FIELD_SERIALIZER_HPP
#define MATADOR_RELATION_FIELD_SERIALIZER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"

#include <string>

namespace matador {

class object_proxy;
class object_holder;
class abstract_has_many;

namespace detail {

class relation_field_serializer
{
public:

  template < class T >
  void set(const std::string &field, T *obj, object_proxy *value);
  template < class T >
  void clear(const std::string &field, T *obj);
  template < class T >
  void append(const std::string &field, T *obj, object_proxy *value);
  template < class T >
  void remove(const std::string &field, T *obj, object_proxy *value);

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
  void execute(serialize_action action, const std::string &field, T *obj, object_proxy *value);
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

template<class T>
void relation_field_serializer::set(const std::string &field, T *obj, object_proxy *value)
{
  execute(SET, field, obj, value);
}

template<class T>
void relation_field_serializer::clear(const std::string &field, T *obj)
{
  execute(CLEAR, field, obj, nullptr);
}

template<class T>
void relation_field_serializer::append(const std::string &field, T *obj, object_proxy *value)
{
  execute(APPEND, field, obj, value);
}

template<class T>
void relation_field_serializer::remove(const std::string &field, T *obj, object_proxy *value)
{
  execute(REMOVE, field, obj, value);
}

template<class T>
void
relation_field_serializer::execute(relation_field_serializer::serialize_action action,
                                   const std::string &field, T *obj, object_proxy *value)
{
  action_ = action;
  value_ = value;
  field_ = field;

  matador::access::serialize(*this, *obj);

  field_ = "";
  value_ = nullptr;
}
}
}

#endif //MATADOR_RELATION_FIELD_SERIALIZER_HPP
