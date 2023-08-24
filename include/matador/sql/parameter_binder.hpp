#ifndef MATADOR_PARAMETER_BINDER_HPP
#define MATADOR_PARAMETER_BINDER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_has_many;
class time;
class date;

template < class T >
class parameter_binder;

namespace detail {

class parameter_binder_impl
{
public:
  virtual ~parameter_binder_impl() = default;
  virtual void reset() = 0;

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

  void serialize(short &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(int &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(long &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(long long &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(unsigned short &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(unsigned int &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(unsigned long &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(unsigned long long &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(std::string &value, const field_attributes &attr) override { bind(value, attr); }
  void serialize(null_type_t &, const field_attributes &attr) override;

private:
  template< typename ValueType >
  void bind(ValueType &value, const field_attributes &attr);

private:
  parameter_binder<Type> &binder_;
};

}
template < class T >
class parameter_binder
{
public:
  parameter_binder(const std::string &id, const T &param, const long size, size_t index, detail::parameter_binder_impl *impl)
    : id_(id)
    , param_(param)
    , size_(size)
    , index_(index)
    , impl_(impl)
  {}

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return index_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < typename V >
  void on_primary_key(const char *id, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) { bind(id, param_, {}); }
  void on_primary_key(const char *id, std::string &, size_t size) { bind(id, param_, size); }
  void on_revision(const char *id, unsigned long long &/*rev*/) { bind(id, param_, {}); }
  void on_attribute(const char *id, char &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, short &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, int &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, long &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, long long &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, unsigned char &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, unsigned short &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, unsigned int &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, unsigned long &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, unsigned long long &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, bool &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, float &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *id, double &, const field_attributes &attr = null_attributes) { bind(id, param_, attr); }
  void on_attribute(const char *, matador::time &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, matador::date &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) {}
  void on_has_one(const char *, identifiable_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  template < class V >
  void bind(const char *id, V &x, const field_attributes &/*attr*/)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(x, index_++);
  }

private:
  const std::string &id_;

  const T &param_;
  const long size_;
  size_t index_{0};

  detail::parameter_binder_impl *impl_;
};

template <>
class parameter_binder<std::string>
{
public:
  parameter_binder(const std::string &id, const std::string &param, const long size, size_t index, detail::parameter_binder_impl *impl)
    : id_(id)
    , param_(param)
    , size_(size)
    , index_(index)
    , impl_(impl)
    , identifier_serializer_(*this)
  {}

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return index_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < typename V >
  void on_primary_key(const char *id, V &v, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) { on_attribute(id, v, {}); }
  void on_primary_key(const char *id, std::string &v, size_t size) { on_attribute(id, v, size); }
  void on_revision(const char *id, unsigned long long &rev) { on_attribute(id, rev, {}); }
  void on_attribute(const char *, char &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, short &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, int &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, long &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, long long &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, unsigned char &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, unsigned short &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, unsigned int &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, unsigned long &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, unsigned long long &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, bool &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, float &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, double &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, matador::time &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, matador::date &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *id, char*, const field_attributes &/*attr*/ = null_attributes) { bind(id, param_.data(), param_.size()); }
  void on_attribute(const char *id, std::string &, const field_attributes &/*attr*/ = null_attributes) { bind(id, param_.data(), param_.size()); }
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
  void bind(const char *id, V &x, long size)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(x, size, index_++);
  }

  void bind(const char *id, const char *data, size_t size)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(data, size, index_++);
  }

private:
  friend class detail::parameter_identifier_serializer<std::string>;

private:
  const std::string &id_;

  const std::string &param_;
  const long size_;
  size_t index_{0};

  detail::parameter_binder_impl *impl_;
  detail::parameter_identifier_serializer<std::string> identifier_serializer_;
};

template <>
class parameter_binder<void>
{
public:
  explicit parameter_binder(size_t index, detail::parameter_binder_impl *impl)
    : index_(index)
    , impl_(impl)
    , identifier_serializer_(*this)
  {}

  template < class V >
  size_t bind(V &obj)
  {
    matador::access::serialize(*this, obj);
    return index_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < typename V >
  void on_primary_key(const char *id, V &v, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) { on_attribute(id, v, {}); }
  void on_primary_key(const char *id, std::string &v, size_t size) { on_attribute(id, v, size); }
  void on_revision(const char *id, unsigned long long &rev) { on_attribute(id, rev, {}); }
  void on_attribute(const char *id, char &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, short &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, int &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, long &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, long long &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned char &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned short &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned int &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned long &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, unsigned long long &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, bool &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, float &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, double &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, matador::time &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *id, matador::date &x, const field_attributes &/*attr*/ = null_attributes) { bind_value(id, x); }
  void on_attribute(const char *, char *x, const field_attributes &attr = null_attributes) { impl_->bind(x, attr.size(), index_++); }
  void on_attribute(const char *, std::string &x, const field_attributes &attr = null_attributes) { impl_->bind(x, attr.size(), index_++); }
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
    impl_->bind(x, index_++);
  }

private:
  size_t index_{0};

  detail::parameter_binder_impl *impl_;
  detail::parameter_identifier_serializer<void> identifier_serializer_;
};

namespace detail {

template<typename Type>
void parameter_identifier_serializer<Type>::serialize(null_type_t &, const field_attributes &/*attr*/)
{
  binder_.impl_->bind_null(true);
}

template<typename Type>
template< typename ValueType >
void parameter_identifier_serializer<Type>::bind(ValueType &value, const field_attributes &/*attr*/)
{
  binder_.impl_->bind(value, binder_.index_++);
}

}
/// @endcond
}
#endif //MATADOR_PARAMETER_BINDER_HPP
