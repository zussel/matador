#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "object/basic_identifier.hpp"
#include "object/object_atomizer.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>
#include <stdexcept>
#include <functional>
#include <memory>

namespace oos {

template<typename T>
class identifier<T, typename std::enable_if<std::is_integral<T>::value>::type> : public basic_identifier
{
public:
  typedef identifier<T> self;

  identifier() : id_(new T(0))
  { };

  explicit identifier(T val) : id_(new T(val))
  { }

//  identifier(const std::shared_ptr<T> &id) : id_(id) { }
//
//  identifier(const basic_identifier *id)
//  {
//    if (!is_same_type(*id)) {
//      throw std::logic_error("not the same type");
//    }
//    const self *xid = static_cast<const self *>(id);
//    *id_ = xid->value();
//  }

  virtual ~identifier()
  { };

  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, *id_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, *id_);
  }

  virtual bool less(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return *id_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual bool equal_to(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return *id_ == static_cast<const identifier<T> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual size_t hash() const
  {
    std::hash<T> pk_hash;
    return pk_hash(*id_);
  }

  virtual bool is_same_type(const basic_identifier &x) const
  { return type_index() == x.type_index(); }

  virtual const std::type_index &type_index() const
  { return type_index_; }

  virtual std::ostream &print(std::ostream &out) const
  {
    out << *id_;
    return out;
  }

  virtual basic_identifier *clone() const
  {
    return new self(*id_);
  }

  virtual basic_identifier *share() const
  {
    return nullptr;
  }

  virtual bool is_valid() const {
    return *id_ != 0;
  }

  T value() const
  { return *id_; }

  void value(T val)
  { *id_ = val; }

private:
  std::shared_ptr<T> id_;

  static std::type_index type_index_;
};

template<typename T>
std::type_index identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>::type_index_ = std::type_index(
    typeid(identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>));


template<>
class identifier<std::string> : public basic_identifier
{
public:
  typedef identifier<std::string> self;

  identifier() : id_(new std::string(""))
  { };

  explicit identifier(const std::string &val) : id_(new std::string(val))
  { }

//  identifier(const std::shared_ptr<std::string> &id) : id_(id) { }
//
//  identifier(const basic_identifier *id)
//  {
//    if (!is_same_type(*id)) {
//      throw std::logic_error("not the same type");
//    }
//    const self *xid = static_cast<const self *>(id);
//    *id_ = xid->value();
//  }

  virtual ~identifier()
  { };

  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, *id_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, *id_);
  }

  virtual bool less(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return *id_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual bool equal_to(const basic_identifier &x) const
  {
    if (this->is_same_type(x)) {
      return *id_ == static_cast<const identifier<std::string> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  virtual size_t hash() const
  {
    std::hash<std::string> pk_hash;
    return pk_hash(*id_);
  }

  virtual bool is_same_type(const basic_identifier &x) const
  { return type_index() == x.type_index(); }

  virtual const std::type_index &type_index() const
  { return type_index_; }

  virtual std::ostream &print(std::ostream &out) const
  {
    out << *id_;
    return out;
  }

  virtual basic_identifier *clone() const
  {
    return new self(*id_);
  }

  virtual basic_identifier *share() const
  {
    return nullptr;
  }

  virtual bool is_valid() const {
    return !id_->empty();
  }

  std::string value() const
  { return *id_; }

  void value(const std::string &val)
  { *id_ = val; }

private:
  std::shared_ptr<std::string> id_;

  static std::type_index type_index_;
};

template<class T>
identifier<T> *make_id(const T &id)
{
  return new identifier<T>(id);
}

template<class T>
identifier<T> share_identifier(const identifier<T> &id)
{
  identifier<T> shared;

  return shared;
}

}

#endif /* IDENTIFIER_HPP */
