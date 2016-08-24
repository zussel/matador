//
// Created by sascha on 7/13/15.
//

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

#include "tools/basic_identifier.hpp"
#include "tools/identifiable_holder.hpp"

#include "sql/statement.hpp"

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

  template < class V, typename = typename std::enable_if<std::is_base_of<oos::identifiable_holder, V>::value>::type >
  void serialize(const char *, V &, cascade_type) { }

  void serialize(const char *, char *, size_t) { }

  template < class HAS_MANY >
  void serialize(const char*, HAS_MANY&, const char*, const char*) {}

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
  stmt_->bind(x.reference(), pos_);
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

