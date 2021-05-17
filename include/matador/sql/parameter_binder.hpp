#ifndef MATADOR_PARAMETER_BINDER_HPP
#define MATADOR_PARAMETER_BINDER_HPP

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_has_many;

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

}
template < class T >
class parameter_binder : public serializer
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

  void serialize(const char *id, char &) override { bind(id, param_); }
  void serialize(const char *id, short &) override { bind(id, param_); }
  void serialize(const char *id, int &) override { bind(id, param_); }
  void serialize(const char *id, long &) override { bind(id, param_); }
  void serialize(const char *id, long long &) override { bind(id, param_); }
  void serialize(const char *id, unsigned char &) override { bind(id, param_); }
  void serialize(const char *id, unsigned short &) override { bind(id, param_); }
  void serialize(const char *id, unsigned int &) override { bind(id, param_); }
  void serialize(const char *id, unsigned long &) override { bind(id, param_); }
  void serialize(const char *id, unsigned long long &) override { bind(id, param_); }
  void serialize(const char *id, bool &) override { bind(id, param_); }
  void serialize(const char *id, float &) override { bind(id, param_); }
  void serialize(const char *id, double &) override { bind(id, param_); }
  void serialize(const char *, matador::time &) override {}
  void serialize(const char *, matador::date &) override {}
  void serialize(const char *, char*, size_t) override {}
  void serialize(const char *, std::string &, size_t) override {}
  void serialize(const char *, std::string &) override {}
  void serialize(const char *id, basic_identifier &x) override { x.serialize(id, *this); }
  void serialize(const char *, identifiable_holder &, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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
class parameter_binder<std::string> : public serializer
{
public:
  parameter_binder(const std::string &id, const std::string &param, size_t index, detail::parameter_binder_impl *impl)
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

  void serialize(const char *, char &) override {}
  void serialize(const char *, short &) override {}
  void serialize(const char *, int &) override {}
  void serialize(const char *, long &) override {}
  void serialize(const char *, long long &) override {}
  void serialize(const char *, unsigned char &) override {}
  void serialize(const char *, unsigned short &) override {}
  void serialize(const char *, unsigned int &) override {}
  void serialize(const char *, unsigned long &) override {}
  void serialize(const char *, unsigned long long &) override {}
  void serialize(const char *, bool &) override {}
  void serialize(const char *, float &) override {}
  void serialize(const char *, double &) override {}
  void serialize(const char *, matador::time &) override {}
  void serialize(const char *, matador::date &) override {}
  void serialize(const char *id, char*, size_t) override { bind(id, param_.data(), param_.size()); }
  void serialize(const char *id, std::string &, size_t) override { bind(id, param_.data(), param_.size()); }
  void serialize(const char *id, std::string &) override { bind(id, param_);}
  void serialize(const char *id, basic_identifier &x) override { x.serialize(id, *this); }
  void serialize(const char *id, identifiable_holder &x, cascade_type) override
  {
    if (x.has_primary_key()) {
      x.primary_key()->serialize(id, *this);
    } else {
      std::unique_ptr<basic_identifier> basic_id(x.create_identifier());
      impl_->bind_null(true);
      basic_id->serialize(id, *this);
      impl_->bind_null(false);
    }
  }
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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
  const std::string &id_;

  const std::string &param_;

  detail::parameter_binder_impl *impl_;
};

template <>
class parameter_binder<void> : public serializer
{
public:
  explicit parameter_binder(size_t index, detail::parameter_binder_impl *impl)
    : impl_(impl)
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

  void serialize(const char *id, char &x) override { bind_value(id, x); }
  void serialize(const char *id, short &x) override { bind_value(id, x); }
  void serialize(const char *id, int &x) override { bind_value(id, x); }
  void serialize(const char *id, long &x) override { bind_value(id, x); }
  void serialize(const char *id, long long &x) override { bind_value(id, x); }
  void serialize(const char *id, unsigned char &x) override { bind_value(id, x); }
  void serialize(const char *id, unsigned short &x) override { bind_value(id, x); }
  void serialize(const char *id, unsigned int &x) override { bind_value(id, x); }
  void serialize(const char *id, unsigned long &x) override { bind_value(id, x); }
  void serialize(const char *id, unsigned long long &x) override { bind_value(id, x); }
  void serialize(const char *id, bool &x) override { bind_value(id, x); }
  void serialize(const char *id, float &x) override { bind_value(id, x); }
  void serialize(const char *id, double &x) override { bind_value(id, x); }
  void serialize(const char *id, matador::time &x) override { bind_value(id, x); }
  void serialize(const char *id, matador::date &x) override { bind_value(id, x); }
  void serialize(const char *id, std::string &x) override { bind_value(id, x); }
  void serialize(const char *, char *x, size_t s) override { impl_->bind(x, s, impl_->next_index()); }
  void serialize(const char *, std::string &x, size_t s) override { impl_->bind(x, s, impl_->next_index()); }
  void serialize(const char *id, basic_identifier &x) override { x.serialize(id, *this); }
  void serialize(const char *id, identifiable_holder &x, cascade_type) override
  {
    if (x.has_primary_key()) {
      x.primary_key()->serialize(id, *this);
    } else {
      std::unique_ptr<basic_identifier> basic_id(x.create_identifier());
      impl_->bind_null(true);
      basic_id->serialize(id, *this);
      impl_->bind_null(false);
    }
  }
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

private:
  template < class V >
  void bind_value(const char *, V &x)
  {
    impl_->bind(x, impl_->next_index());
  }

private:
  detail::parameter_binder_impl *impl_;
};

/// @endcond
}
#endif //MATADOR_PARAMETER_BINDER_HPP
