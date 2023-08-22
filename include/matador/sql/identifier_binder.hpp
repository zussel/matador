#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

#include "matador/utils/identifier.hpp"

#include "matador/object/object_exception.hpp"

#include "matador/sql/statement.hpp"

namespace matador {

class object_holder;
class abstract_has_many;
class identifiable_holder;

namespace detail {
/// @cond MATADOR_DEV

template < class T >
class identifier_binder : public identifier_serializer
{
public:
  identifier_binder() = default;
  ~identifier_binder() override = default;

  void bind(T *obj, statement<T> *stmt, size_t pos, identifier &id);

  template<class V>
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &x, const field_attributes &/*attr*/ = null_attributes);

  template<class V>
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}

  void on_belongs_to(const char *, identifiable_holder &, cascade_type) { }
  void on_has_one(const char *, identifiable_holder &, cascade_type) { }

  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) { }
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) { }

  void on_has_many(const char*, abstract_has_many&, const char*, const char*, cascade_type) {}
  void on_has_many(const char*, abstract_has_many&, cascade_type) {}

  void serialize(short &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(int &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(long &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(long long int &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(unsigned short &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(unsigned int &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(unsigned long &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(unsigned long long int &i, const field_attributes &/*attr*/) override
  {
    bind(i);
  }

  void serialize(std::string &str, const field_attributes &attr) override
  {
    if (!stmt_->p->is_valid_host_var(field_name_, pos_)) {
      return;
    }
    stmt_->bind(pos_, str, attr.size());
  }

  void serialize(null_type_t &/*type*/, const field_attributes &/*attr*/) override { }

private:
  void setup(statement<T> *stmt, T *obj, size_t pos, identifier &id);
  void cleanup();

  template<class Type>
  void bind(Type &val) {
    if (pos_ >= stmt_->p->bind_vars().size()) {
      throw std::out_of_range("host index out of range");
    }

    if (!stmt_->p->is_valid_host_var(field_name_, pos_)) {
      return;
    }
    stmt_->bind(pos_, val);
  }

private:
  statement<T> *stmt_ = nullptr;
  size_t pos_ = 0;
  T *obj_ = nullptr;
  identifier *id_ = nullptr;
  std::string field_name_;
};

template<class T>
void identifier_binder<T>::bind(T *obj, statement<T> *stmt, size_t pos, identifier &id)
{
  setup(stmt, obj, pos, id);

  matador::access::serialize(*this, *obj);

  cleanup();
}

template < class T >
template< class V >
void identifier_binder<T>::on_primary_key(const char *id, V &/*x*/, const field_attributes &/*attr*/)
{
  if (!id_->is_similar_type<V>()) {
    throw_object_exception("identifier types aren't equal");
  }
  field_name_ = id;
  id_->serialize(*this);
}

template < class T >
void identifier_binder<T>::setup(statement <T> *stmt, T *obj, size_t pos, identifier &id)
{
  stmt_ = stmt;
  pos_ = pos;
  obj_ = obj;
  id_ = &id;
}

template < class T >
void identifier_binder<T>::cleanup()
{
  stmt_ = nullptr;
  pos_ = 0;
  obj_ = nullptr;
  id_ = nullptr;
  field_name_.clear();
}

/// @endcond
}
}

#endif /* PRIMARY_KEY_BINDER_HPP */

