#ifndef ATTRIBUTE_SERIALIZER_HPP
#define ATTRIBUTE_SERIALIZER_HPP

#include "matador/object/object_ptr.hpp"

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/string.hpp"

#include <stdexcept>
#include <type_traits>
#include <cstring>

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_has_many;

class basic_attribute_serializer
{
public:
  bool success() const
  {
    return success_;
  }

protected:
  explicit basic_attribute_serializer(std::string id) : id_(std::move(id)) {}

protected:
  std::string id_;
  bool success_ = false;
};

/**
 * @tparam T The type of the attribute to set.
 * @class attribute_reader
 * @brief Set an attribute value of an serializable.
 *
 * A given attribute value of a template type is
 * tried to set into an serializable. Therefor the attribute
 * with given name must be found and the value must
 * be convertible into the objects attribute.
 */
template < class T >
class attribute_reader : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const T &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void on_primary_key(const char *id, V &to, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    on_attribute(id, to);
  }
  void on_primary_key(const char *id, std::string &to, size_t size)
  {
    on_attribute(id, to, size);
  }

  void on_revision(const char *id, unsigned long long &rev)
  {
    on_attribute(id, rev);
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = (V)from_;
    this->success_ = true;
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<T>::value && std::is_same<V, bool>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_ > 0;
    this->success_ = true;
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<!std::is_arithmetic<T>::value && std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<(!std::is_arithmetic<T>::value || !std::is_arithmetic<V>::value) && !std::is_same<T, V>::value >::type* = 0) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, object_holder &, cascade_type) {}
  void on_has_one(const char *, object_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  const T &from_;
};

template <>
class attribute_reader<bool> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, bool from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t) {}
  void on_revision(const char *id, unsigned long long &rev)
  {
    on_attribute(id, rev);
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = (from_ ? 1 : 0);
    this->success_ = true;
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = (from_ ? 1.0f : 0.0f);
    this->success_ = true;
  }

  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<V>::value && std::is_same<bool, V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }

  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<!std::is_arithmetic<V>::value>::type* = 0) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, object_holder &, cascade_type) {}
  void on_has_one(const char *, object_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  bool from_;
};

template < class T >
class attribute_reader<object_ptr<T>> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const object_ptr<T> &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  template < class V >
  void on_belongs_to(const char *, object_ptr<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    x = from_;
    this->success_ = true;
  }
  template < class V >
  void on_has_one(const char *, object_ptr<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    x = from_;
    this->success_ = true;
  }
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  const object_ptr<T> &from_;
};

template < class T, class Enabled = void >
class has_many_attribute_reader;

template < class T >
class has_many_attribute_reader<T, typename std::enable_if<!std::is_base_of<object_holder, T>::value>::type> : public basic_attribute_serializer
{
public:
  has_many_attribute_reader(const std::string &id, const T &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) { }
  void on_has_many(const char *, identifiable_holder &, cascade_type) { }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, const char *, const char *, cascade_type)
  {
    x.push_back(from_);
    this->success_ = true;
  }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, cascade_type)
  {
    x.push_back(from_);
    this->success_ = true;
  }

private:
  const T &from_;
};

template < class T >
class has_many_attribute_reader<object_ptr<T>> : public basic_attribute_serializer
{
public:
  has_many_attribute_reader(const std::string &id, const object_ptr<T> &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) { }
  void on_has_many(const char *, identifiable_holder &, cascade_type) { }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, const char *, const char *, cascade_type)
  {
    x.push_back(from_);
    this->success_ = true;
  }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, cascade_type)
  {
    x.push_back(from_);
    this->success_ = true;
  }

private:
  const object_ptr<T> &from_;
};

template <>
class attribute_reader<const char*> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const char* from)
    : basic_attribute_serializer(id)
    , from_(from)
    , len_(static_cast<long>(strlen(from)))
  {}

  ~attribute_reader() = default;

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *id, std::string &to, size_t size)
  {
    on_attribute(id, to, size);
  }
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }
  void on_attribute(const char *id, char *to, const field_attributes &attr)
  {
    if (id_ != id) {
      return;
    }
    if (attr.size() > len_) {
#ifdef _MSC_VER
		strncpy_s(to, attr.size(), from_, len_);
		to[len_] = '\0';
#else
		strcpy(to, from_);
#endif
      this->success_ = true;
    }
  }
  void on_belongs_to(const char *, object_holder &, cascade_type) {}
  void on_has_one(const char *, object_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  const char* from_ = nullptr;
  size_t len_ = 0;
};

template <>
class attribute_reader<std::string> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const std::string &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  ~attribute_reader() = default;

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *id, std::string &to, size_t size)
  {
    on_attribute(id, to, size);
  }
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }
  void on_attribute(const char *id, char *to, const field_attributes &attr)
  {
    if (id_ != id) {
      return;
    }
    if (attr.size() > from_.size()) {
#ifdef _MSC_VER
		strncpy_s(to, attr.size(), from_.c_str(), from_.size());
		to[from_.size()] = '\0';
#else
		strncpy(to, from_.c_str(), from_.size());
#endif
      this->success_ = true;
    }
  }
  void on_belongs_to(const char *, object_holder &, cascade_type) {}
  void on_has_one(const char *, object_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  const std::string &from_;
};

template < class T >
class attribute_writer : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, T &to, size_t precision = 0)
    : basic_attribute_serializer(id)
    , to_(to)
    , precision_(precision)
  {}

  ~attribute_writer() = default;

  template < class V >
  void on_primary_key(const char *id, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    on_attribute(id, from);
  }
  void on_primary_key(const char *id, std::string &from, size_t size)
  {
    on_attribute(id, from, size);
  }
  void on_revision(const char *id, unsigned long long &rev)
  {
    on_attribute(id, rev);
  }

  template < class V >
  void on_attribute(const char *id, V &from, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if< std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && !std::is_same<bool, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = (T)from;
    success_ = true;
  }
  template < class V >
  void on_attribute(const char *id, V &from, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && std::is_same<bool, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = (from ? 1 : 0);
    success_ = true;
  }
  template < class V >
  void on_attribute(const char *id, V &from, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<!std::is_arithmetic<T>::value && std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<(!std::is_arithmetic<T>::value || !std::is_arithmetic<V>::value) &&  !std::is_same<T, V>::value >::type* = 0) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, object_holder &, cascade_type) {}
  void on_has_one(const char *, object_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  T &to_;
  size_t precision_;
};

template < class T >
class attribute_writer<object_ptr<T>> : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, object_ptr<T> &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void on_primary_key(const char *, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &/*from*/, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  template < class V >
  void on_belongs_to(const char *, object_ptr<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    to_ = x;
  }
  template < class V >
  void on_has_one(const char *, object_ptr<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    to_ = x;
  }
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  object_ptr<T> &to_;
};

template < class T, class Enabled = void >
class has_many_attribute_writer;

template < class T >
class has_many_attribute_writer<T, typename std::enable_if<!std::is_base_of<object_holder, T>::value>::type> : public basic_attribute_serializer
{
public:
  has_many_attribute_writer(const std::string &id, const T &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &/*from*/, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) {}
  void on_has_one(const char *, identifiable_holder &, cascade_type) {}
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, const char *, const char *, cascade_type)
  {
    x.remove(to_);
    this->success_ = true;
  }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, cascade_type)
  {
    x.remove(to_);
    this->success_ = true;
  }
private:
  const T &to_;
};

template < class T >
class has_many_attribute_writer<object_ptr<T>> : public basic_attribute_serializer
{
public:
  has_many_attribute_writer(const std::string &id, const object_ptr<T> &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &/*from*/, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char*, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) {}
  void on_has_many(const char *, identifiable_holder &, cascade_type) {}
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, const char *, const char *, cascade_type)
  {
    x.remove(to_);
    this->success_ = true;
  }
  template<class V, template <class ...> class C>
  void on_has_many(const char *, has_many<V, C> &x, cascade_type)
  {
    x.remove(to_);
    this->success_ = true;
  }
private:
  const object_ptr<T> &to_;
};

template <>
class attribute_writer<std::string> : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, std::string &to, size_t precision = 0)
    : basic_attribute_serializer(id)
    , to_(to)
    , precision_(precision)
  {}

  ~attribute_writer() = default;

  template < class V >
  void on_primary_key(const char *, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *id, std::string &from, size_t size)
  {
    on_attribute(id, from, size);
  }
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class V >
  void on_attribute(const char *id, V &from, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if< !std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = std::to_string(from);
    success_ = true;
  }

  template < class V >
  void on_attribute(const char *id, V &from, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if< std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = matador::to_string(from, precision_);
    success_ = true;
  }

  void on_attribute(const char *id, std::string &from, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }

  void on_attribute(const char *id, char *from, const field_attributes &attr)
  {
    if (id_ != id) {
      return;
    }
    to_.assign(from, attr.size());
    success_ = true;
  }

  void on_attribute(const char *id, date &from, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }
  void on_attribute(const char *id, matador::time &from, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }

  template < class V >
  void on_belongs_to(const char *id, object_ptr<V> &x, cascade_type)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    if (x.has_primary_key()) {
      x.primary_key()->print(to);
    } else {
      to << x.id();
    }
    to_ = to.str();
    success_ = true;
  }

  template < class V >
  void on_has_one(const char *id, object_ptr<V> &x, cascade_type)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    if (x.has_primary_key()) {
      x.primary_key()->print(to);
    } else {
      to << x.id();
    }
    to_ = to.str();
    success_ = true;
  }

  void on_has_many(const char*, abstract_has_many&, const char*, const char*, cascade_type) {}
  void on_has_many(const char*, abstract_has_many&, cascade_type) {}

private:
  std::string &to_;
  size_t precision_;
};

template <>
class attribute_writer<char*>
{
public:
  /**
   * @brief Creates an attribute_writer
   *
   * Creates an attribute_writer for an attribute id of type T
   * where id is the name of the attribute.
   *
   * @tparam T The type of the attribute.
   * @param id The name of the attribute.
   * @param to The attribute value to retrieve.
   */
  attribute_writer(std::string id, char *to, long size)
    : id_(std::move(id))
    , to_(to)
    , size_(size)
    , success_(false)
  {}

  ~attribute_writer() = default;

  /**
   * @brief True if value could be retrieved.
   *
   * Returns true if the value could
   * be retrieved successfully.
   *
   * @return True if value could be retrieved.
   */
  bool success() const
  {
    return success_;
  }

  template < class V >
  void on_primary_key(const char *id, V &from, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    on_attribute(id, from);
  }
  void on_primary_key(const char *id, std::string &from, size_t size)
  {
    on_attribute(id, from, size);
  }
  void on_revision(const char *id, unsigned long long &rev)
  {
    on_attribute(id, rev);
  }
  template < class V >
  void on_attribute(const char *id, V &/*from*/, const field_attributes &/*attr*/ = null_attributes)
  {
    if (id_ != id) {
      return;
    }
    // Todo: convert each type to char*
    success_ = true;
  }
  void on_attribute(const char*, date&, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char*, time&, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char*, identifiable_holder &, cascade_type) {}
  void on_has_one(const char*, identifiable_holder &, cascade_type) {}
  void on_has_many(const char*, abstract_has_many&, const char*, const char*, cascade_type) {}
  void on_has_many(const char*, abstract_has_many&, cascade_type) {}

  void on_attribute(const char *id, std::string &from, const field_attributes &attr)
  {
    if (id_ != id) {
      return;
    }
    if (size_ < attr.size()) {
      // not enough size
      return;
    }
#ifdef _MSC_VER
    strncpy_s(to_, size_, from.c_str(), attr.size());
#else
    strncpy(to_, from.c_str(), attr.size());
#endif
    size_t from_size = from.size();
    if (from_size > attr.size()) {
      to_[attr.size()-1] = '\0';
    } else {
      to_[from_size] = '\0';
    }
    success_ = true;
  }

  void on_attribute(const char *id, char *from, const field_attributes &attr)
  {
    if (id_ != id) {
      return;
    }
    if (size_ < attr.size()) {
      // not enough size
      return;
    }
#ifdef _MSC_VER
    strncpy_s(to_, size_, from, attr.size());
#else
    strncpy(to_, from, attr.size());
#endif
    size_t from_size = strlen(from);
    if (from_size > attr.size()) {
      to_[attr.size()-1] = '\0';
    } else {
      to_[from_size] = '\0';
    }
    success_ = true;
  }

private:
  std::string id_;
  char *to_;
  size_t size_;
  bool success_;
};

/// @endcond

}

#endif /* ATTRIBUTE_SERIALIZER_HPP */
