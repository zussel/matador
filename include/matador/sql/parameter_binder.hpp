//
// Created by sascha on 30.08.19.
//

#ifndef MATADOR_PARAMETER_BINDER_HPP
#define MATADOR_PARAMETER_BINDER_HPP

#include "matador/utils/cascade_type.hpp"

#include <string>

namespace matador {

class object_holder;
class abstract_has_many;

namespace detail {

class parameter_binder_impl
{
public:
  virtual void bind(char, size_t) = 0;
  virtual void bind(short, size_t) = 0;
  virtual void bind(int, size_t) = 0;
  virtual void bind(long, size_t) = 0;
  virtual void bind(unsigned char, size_t) = 0;
  virtual void bind(unsigned short, size_t) = 0;
  virtual void bind(unsigned int, size_t) = 0;
  virtual void bind(unsigned long, size_t) = 0;
  virtual void bind(bool, size_t) = 0;
  virtual void bind(float, size_t) = 0;
  virtual void bind(double, size_t) = 0;
  virtual void bind(const char *, size_t, size_t) = 0;
  virtual void bind(const std::string&, size_t) = 0;
  virtual void bind(const std::string &x, size_t s, size_t) = 0;
  virtual void bind(const matador::time&, size_t) = 0;
  virtual void bind(const matador::date&, size_t) = 0;
  virtual void bind(const matador::basic_identifier &x, size_t) = 0;
  virtual void bind(const matador::identifiable_holder &x, cascade_type, size_t) = 0;
};

}
template < class T >
class parameter_binder
{
public:
  parameter_binder(const std::string &id, T &param, detail::parameter_binder_impl *impl)
    : id_(id), param_(param), impl_(impl)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *id, V &, typename std::enable_if< std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && !std::is_same<bool, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(param_, index_);
  }

  template < class V >
  void serialize(const char *id, V &, typename std::enable_if<std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && std::is_same<bool, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(param_, index_);
//    param_ = (from ? 1 : 0);
  }

  template < class V >
  void serialize(const char *id, V &, typename std::enable_if<!std::is_arithmetic<T>::value && std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(param_, index_);
  }
  template < class V >
  void serialize(const char *, V &, typename std::enable_if<(!std::is_arithmetic<T>::value || !std::is_arithmetic<V>::value) &&  !std::is_same<T, V>::value >::type* = 0) {}
  void serialize(const char *, char*, size_t) {}
  void serialize(const char *, std::string &, size_t) {}
  void serialize(const char *, object_holder &, cascade_type) {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void serialize(const char *, abstract_has_many &, cascade_type) {}

private:
  const std::string &id_;

  T &param_;

  size_t index_ = 0;
  detail::parameter_binder_impl *impl_;
};

template <>
class parameter_binder<std::string>
{
public:
  parameter_binder(const std::string &id, std::string &param, detail::parameter_binder_impl *impl)
    : id_(id), param_(param), impl_(impl)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}

  void serialize(const char *id, char*, size_t)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(param_.data(), param_.size(), index_);
  }
  void serialize(const char *id, std::string &, size_t)
  {
    if (id_ != id) {
      return;
    }
    impl_->bind(param_.data(), param_.size(), index_);
  }
  void serialize(const char *, object_holder &, cascade_type) {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void serialize(const char *, abstract_has_many &, cascade_type) {}

private:
  const std::string &id_;

  std::string &param_;

  size_t index_ = 0;

  detail::parameter_binder_impl *impl_;
};

}
#endif //MATADOR_PARAMETER_BINDER_HPP
