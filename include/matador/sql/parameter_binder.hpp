#ifndef MATADOR_PARAMETER_BINDER_HPP
#define MATADOR_PARAMETER_BINDER_HPP

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_has_many;

template < class T >
class parameter_binder;

namespace detail {

class parameter_binder_impl
{
public:
  virtual ~parameter_binder_impl() = default;
  virtual void reset() = 0;

  virtual void initialize_index(size_t index) = 0;
  virtual size_t next_index() = 0;
  virtual size_t current_index() const = 0;

  virtual void bind(char, size_t) = 0;
  virtual void bind(short, size_t) = 0;
  virtual void bind(int, size_t) = 0;
  virtual void bind(long, size_t) = 0;
  virtual void bind(long long, size_t) = 0;
  virtual void bind(unsigned char, size_t) = 0;
  virtual void bind(unsigned short, size_t) = 0;
  virtual void bind(unsigned int, size_t) = 0;
  virtual void bind(unsigned long, size_t) = 0;
  virtual void bind(unsigned long long, size_t) = 0;
  virtual void bind(bool, size_t) = 0;
  virtual void bind(float, size_t) = 0;
  virtual void bind(double, size_t) = 0;
  virtual void bind(const char *, size_t, size_t) = 0;
  virtual void bind(const std::string&, size_t) = 0;
  virtual void bind(const std::string &x, size_t s, size_t) = 0;
  virtual void bind(const matador::time&, size_t) = 0;
  virtual void bind(const matador::date&, size_t) = 0;

  void bind_null(bool is_null)
  {
    bind_null_ = is_null;
  }

protected:
  bool bind_null_ = false;
};

template < typename Type >
class parameter_identifier_serializer : public identifier_serializer
{
public:
  explicit parameter_identifier_serializer(parameter_binder<Type> &binder)
  : binder_(binder) {}

  void serialize(short &value) override { bind(value); }
  void serialize(int &value) override { bind(value); }
  void serialize(long &value) override { bind(value); }
  void serialize(long long &value) override { bind(value); }
  void serialize(unsigned short &value) override { bind(value); }
  void serialize(unsigned int &value) override { bind(value); }
  void serialize(unsigned long &value) override { bind(value); }
  void serialize(unsigned long long &value) override { bind(value); }
  void serialize(std::string &value) override { bind(value); }
  void serialize(null_type_t &) override;

private:
  template< typename ValueType >
  void bind(ValueType &value);

private:
  parameter_binder<Type> &binder_;
};

}
template < class T >
class parameter_binder
{
public:
  parameter_binder(const std::string &id, const T &param, size_t index, detail::parameter_binder_impl *impl)
    : id_(id), param_(param), impl_(impl)
  {
    impl_->initialize_index(index);
  }

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return impl_->current_index();
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_attribute(const char *id, char &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, short &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, int &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, long &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, long long &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, unsigned char &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, unsigned short &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, unsigned int &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, unsigned long &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, unsigned long long &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, bool &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, float &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *id, double &, long /*size*/ = -1) { bind(id, param_); }
  void on_attribute(const char *, matador::time &, long /*size*/ = -1) {}
  void on_attribute(const char *, matador::date &, long /*size*/ = -1) {}
  void on_attribute(const char *, char*, long /*size*/ = -1) {}
  void on_attribute(const char *, std::string &, long /*size*/ = -1) {}
  template < typename V >
  void on_primary_key(const char *id, V &, long) { bind(id, param_); }
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) {}
  void on_has_one(const char *, identifiable_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  template < class V >
  void bind(const char *id, V &x)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(x, impl_->next_index());
  }

private:
  const std::string &id_;

  const T &param_;

  detail::parameter_binder_impl *impl_;
};

template <>
class parameter_binder<std::string>
{
public:
  parameter_binder(const std::string &id, const std::string &param, size_t index, detail::parameter_binder_impl *impl)
    : id_(id)
    , param_(param)
    , impl_(impl)
    , identifier_serializer_(*this)
  {
    impl_->initialize_index(index);
  }

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return impl_->current_index();
  }

  void on_attribute(const char *, char &, long) {}
  void on_attribute(const char *, short &, long) {}
  void on_attribute(const char *, int &, long) {}
  void on_attribute(const char *, long &, long) {}
  void on_attribute(const char *, long long &, long) {}
  void on_attribute(const char *, unsigned char &, long) {}
  void on_attribute(const char *, unsigned short &, long) {}
  void on_attribute(const char *, unsigned int &, long) {}
  void on_attribute(const char *, unsigned long &, long) {}
  void on_attribute(const char *, unsigned long long &, long) {}
  void on_attribute(const char *, bool &, long) {}
  void on_attribute(const char *, float &, long) {}
  void on_attribute(const char *, double &, long) {}
  void on_attribute(const char *, matador::time &, long) {}
  void on_attribute(const char *, matador::date &, long) {}
  void on_attribute(const char *id, char*, size_t) { bind(id, param_.data(), param_.size()); }
  void on_attribute(const char *id, std::string &, size_t) { bind(id, param_.data(), param_.size()); }
  void on_attribute(const char *id, std::string &, long) { bind(id, param_);}
  template < typename V >
  void on_primary_key(const char *id, V &v, long s) { on_attribute(id, v, s); }
  void on_belongs_to(const char */*id*/, identifiable_holder &x, cascade_type)
  {
    if (x.has_primary_key()) {
      x.primary_key().serialize(identifier_serializer_);
    } else {
      auto pk = x.create_identifier();
      impl_->bind_null(true);
      pk.serialize(identifier_serializer_);
      impl_->bind_null(false);
    }
  }
  void on_has_one(const char */*id*/, identifiable_holder &x, cascade_type)
  {
    if (x.has_primary_key()) {
      x.primary_key().serialize(identifier_serializer_);
    } else {
      auto pk = x.create_identifier();
      impl_->bind_null(true);
      pk.serialize(identifier_serializer_);
      impl_->bind_null(false);
    }
  }
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  template < class V >
  void bind(const char *id, V &x)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(x, impl_->next_index());
  }

  void bind(const char *id, const char *data, size_t size)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(data, size, impl_->next_index());
  }

private:
  friend class detail::parameter_identifier_serializer<std::string>;

private:
  const std::string &id_;

  const std::string &param_;

  detail::parameter_binder_impl *impl_;
  detail::parameter_identifier_serializer<std::string> identifier_serializer_;
};

template <>
class parameter_binder<void>
{
public:
  explicit parameter_binder(size_t index, detail::parameter_binder_impl *impl)
    : impl_(impl)
    , identifier_serializer_(*this)
  {
    impl_->initialize_index(index);
  }

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return impl_->current_index();
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_attribute(const char *id, char &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, short &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, int &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, long &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, long long &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned char &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned short &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned int &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned long &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned long long &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, bool &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, float &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, double &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, matador::time &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *id, matador::date &x, long /*size*/ = -1) { bind_value(id, x); }
  void on_attribute(const char *, char *x, long size = -1) { impl_->bind(x, size, impl_->next_index()); }
  void on_attribute(const char *, std::string &x, long size = -1) { impl_->bind(x, size, impl_->next_index()); }
  template< typename V >
  void on_primary_key(const char *id, V &x, long size = -1) { on_attribute(id, x, size); }
  void on_belongs_to(const char */*id*/, identifiable_holder &x, cascade_type)
  {
    if (x.has_primary_key()) {
      x.primary_key().serialize(identifier_serializer_);
    } else {
      auto pk = x.create_identifier();
      impl_->bind_null(true);
      pk.serialize(identifier_serializer_);
      impl_->bind_null(false);
    }
  }
  void on_has_one(const char */*id*/, identifiable_holder &x, cascade_type)
  {
    if (x.has_primary_key()) {
      x.primary_key().serialize(identifier_serializer_);
    } else {
      auto pk = x.create_identifier();
      impl_->bind_null(true);
      pk.serialize(identifier_serializer_);
      impl_->bind_null(false);
    }
  }
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  friend class detail::parameter_identifier_serializer<void>;

private:
  template < class V >
  void bind_value(const char *, V &x)
  {
    impl_->bind(x, impl_->next_index());
  }

private:
  detail::parameter_binder_impl *impl_;
  detail::parameter_identifier_serializer<void> identifier_serializer_;
};

namespace detail {

template<typename Type>
void parameter_identifier_serializer<Type>::serialize(null_type_t &)
{
  binder_.impl_->bind_null(true);
}

template<typename Type>
template< typename ValueType >
void parameter_identifier_serializer<Type>::bind(ValueType &value)
{
  binder_.impl_->bind(value, binder_.impl_->next_index());
}

}
/// @endcond
}
#endif //MATADOR_PARAMETER_BINDER_HPP
