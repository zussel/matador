//
// Created by sascha on 7/13/15.
//

#include "tools/basic_identifier.hpp"

#include "sql/statement.hpp"

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

namespace oos {

namespace detail {
/// @cond OOS_DEV

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
    oos::access::serialize(*this, x);
  }

  template<class V>
  void serialize(const char *, V &) {}

  template < class V >
  void serialize(const char *, identifier<V> &x);

  void serialize(const char *, char *, size_t) { }

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

  oos::access::serialize(*this, *obj);

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

