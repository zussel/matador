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
  identifier_binder() = default;
  virtual ~identifier_binder() = default;

  void bind(T *obj, statement<T> *stmt, size_t pos, basic_identifier *id);

  template<class V>
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template<class V>
  void on_attribute(const char *, V &, long /*size*/ = -1) {}

  template < class V >
  void on_primary_key(const char *, V &x, long size = -1);

  void on_belongs_to(const char *, identifiable_holder &, cascade_type) { }
  void on_has_one(const char *, identifiable_holder &, cascade_type) { }

  void on_attribute(const char *, char *, size_t) { }
  void on_attribute(const char *, std::string &, size_t) { }

  void on_has_many(const char*, abstract_has_many&, const char*, const char*, cascade_type) {}
  void on_has_many(const char*, abstract_has_many&, cascade_type) {}

private:
  void setup(statement<T> *stmt, T *obj, size_t pos, basic_identifier *id);

  void cleanup();

private:
  statement<T> *stmt_ = nullptr;
  size_t pos_ = 0;
  T *obj_ = nullptr;
  basic_identifier* id_ = nullptr;
};

template<class T>
void identifier_binder<T>::bind(T *obj, statement<T> *stmt, size_t pos, basic_identifier *id)
{
  setup(stmt, obj, pos, id);

  matador::access::serialize(*this, *obj);

  cleanup();
}

template < class T >
template< class V >
void identifier_binder<T>::on_primary_key(const char *, V &/*x*/, long /*size*/)
{
  throw_object_exception("needs implementation");
//  if (!x.is_same_type(*id_)) {
//    throw_object_exception("identifier types aren't equal");
//  }
//  stmt_->bind(pos_, static_cast<identifier<V>*>(id_)->reference());
}

template < class T >
void identifier_binder<T>::setup(statement <T> *stmt, T *obj, size_t pos, basic_identifier *id)
{
  stmt_ = stmt;
  pos_ = pos;
  obj_ = obj;
  id_ = id;
}

template < class T >
void identifier_binder<T>::cleanup()
{
  stmt_ = nullptr;
  pos_ = 0;
  obj_ = nullptr;
  id_ = nullptr;
}

/// @endcond
}
}

#endif /* PRIMARY_KEY_BINDER_HPP */

