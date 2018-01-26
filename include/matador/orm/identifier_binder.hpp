//
// Created by sascha on 7/13/15.
//

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include "matador/sql/statement.hpp"

namespace matador {

class object_holder;
class abstract_has_many;

namespace detail {
/// @cond MATADOR_DEV

template < class T >
class identifier_binder
{
public:
  identifier_binder() { }

  virtual ~identifier_binder() { }

  void bind(T *obj, statement<T> *stmt, size_t pos);

  template<class V>
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template<class V>
  void serialize(const char *, V &) {}

  template < class V >
  void serialize(const char *, identifier<V> &x);

//  template < class V, typename = typename std::enable_if<std::is_base_of<matador::identifiable_holder, V>::value>::type >
//  template < class HAS_ONE >
  void serialize(const char *, object_holder &, cascade_type) { }

  void serialize(const char *, char *, size_t) { }

//  template < class HAS_MANY >
  void serialize(const char*, abstract_has_many&, const char*, const char*, cascade_type) {}
  void serialize(const char*, abstract_has_many&, cascade_type) {}

private:
  void setup(statement<T> *stmt, T *obj, size_t pos);

  void cleanup();

private:
  statement<T> *stmt_ = nullptr;
  size_t pos_ = 0;
  T *obj_ = nullptr;
};

template<class T>
void identifier_binder<T>::bind(T *obj, statement<T> *stmt, size_t pos)
{
  setup(stmt, obj, pos);

  matador::access::serialize(*this, *obj);

  cleanup();
}

template < class T >
template< class V >
void identifier_binder<T>::serialize(const char *, identifier<V> &x)
{
  stmt_->bind(pos_, x.reference());
}

template < class T >
void identifier_binder<T>::setup(statement <T> *stmt, T *obj, size_t pos)
{
  stmt_ = stmt;
  pos_ = pos;
  obj_ = obj;
}

template < class T >
void identifier_binder<T>::cleanup()
{
  stmt_ = nullptr;
  pos_ = 0;
  obj_ = nullptr;
}

/// @endcond
}
}

#endif /* PRIMARY_KEY_BINDER_HPP */

