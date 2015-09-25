#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "object/basic_identifier.hpp"
#include "object/object_atomizer.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>
#include <stdexcept>
#include <functional>

namespace oos {

template<typename T>
class identifier<T, typename std::enable_if<std::is_integral<T>::value>::type> : public basic_identifier
{
public:
  typedef identifier<T> self;

  identifier()
  { };

  explicit identifier(T val) : value_(val)
  { }

  virtual ~identifier()
  { };

  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, value_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, value_);
  }

  virtual bool less(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return value_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual bool equal_to(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return value_ == static_cast<const identifier<T> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual size_t hash() const
  {
    std::hash<T> pk_hash;
    return pk_hash(value_);
  }

  virtual bool is_same_type(const basic_identifier &x) const
  { return type_index() == x.type_index(); }

  virtual const std::type_index &type_index() const
  { return type_index_; }

  virtual std::ostream &print(std::ostream &out) const
  {
    out << value_;
    return out;
  }

  virtual basic_identifier *clone() const
  {
    return new self(value_);
  }

  virtual bool is_valid() const {
    return value_ != 0;
  }

  T value() const
  { return value_; }

  void value(T val)
  { value_ = val; }

private:
  T value_ = 0;

  static std::type_index type_index_;
};

template<typename T>
std::type_index identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>::type_index_ = std::type_index(
    typeid(identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>));


template<typename T>
class identifier<T, typename std::enable_if<std::is_same<T, std::string>::value>::type> : public basic_identifier
{
public:
  typedef identifier<T> self;

  identifier()
  { };

  explicit identifier(T val) : value_(val)
  { }

  virtual ~identifier()
  { };

  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, value_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, value_);
  }

  virtual bool less(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return value_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual bool equal_to(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return value_ == static_cast<const identifier<T> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual size_t hash() const
  {
    std::hash<T> pk_hash;
    return pk_hash(value_);
  }

  virtual bool is_same_type(const basic_identifier &x) const
  { return type_index() == x.type_index(); }

  virtual const std::type_index &type_index() const
  { return type_index_; }

  virtual std::ostream &print(std::ostream &out) const
  {
    out << value_;
    return out;
  }

  virtual basic_identifier *clone() const
  {
    return new self(value_);
  }

  virtual bool is_valid() const {
    return !value_.empty();
  }

  T value() const
  { return value_; }

  void value(T val)
  { value_ = val; }

private:
  T value_;

  static std::type_index type_index_;
};

template<typename T>
std::type_index identifier<T, typename std::enable_if<std::is_same<T, std::string>::value>::type>::type_index_ = std::type_index(
    typeid(identifier<T, typename std::enable_if<std::is_same<T, std::string>::value>::type>));

template<class T>
identifier<T> *make_id(const T &id)
{
  return new identifier<T>(id);
}

}

#endif /* IDENTIFIER_HPP */
