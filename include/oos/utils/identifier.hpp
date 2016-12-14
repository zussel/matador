#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
#define OOS_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define OOS_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include "oos/utils/basic_identifier.hpp"
#include "oos/utils/serializer.hpp"

#include <type_traits>
#include <stdexcept>
#include <functional>
#include <memory>

namespace oos {

/**
 * @brief Identifier class for integral types
 *
 * @tparam T Type of the identifier
 */
template<typename T>
class identifier<T, typename std::enable_if<std::is_integral<T>::value>::type> : public basic_identifier
{
public:
  typedef identifier<T> self; /**< Shortcut to self */
  typedef T type;             /**< Shortcut to type */

  /**
   * @brief Create an identifier
   */
  identifier() : id_(new T(0)) { };

  /**
   * @brief Create an identifier with given value
   * @param val Value of the identifier
   */
  identifier(T val) : id_(new T(val)) { }

  /**
   * @brief Copy assigns a new identifier from given value
   * @param val Value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(T val)
  {
    *id_ = val;
    return *this;
  }

  virtual ~identifier() { };

  /**
   * Serialize the identifier value with
   * the given serializer.
   * 
   * @param id Name of the identifier
   * @param s The serializer to serialize with
   */
  virtual void serialize(const char *id, serializer &s) override
  {
    s.serialize(id, *id_);
  }

  /**
   * Returns true if own id value is less
   * than foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is less than foreign id value
   */
  virtual bool less(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return *id_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  /**
   * Returns true if own id value is equal to
   * foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is equal to foreign id value
   */
  virtual bool equal_to(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return *id_ == static_cast<const identifier<T> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  /**
   * Create a hash value of current
   * id value
   * 
   * @return The calculated hash value
   */
  virtual size_t hash() const override
  {
    std::hash<T> pk_hash;
    return pk_hash(*id_);
  }

  /**
   * Returns true if foreign id type is
   * the same as this id type
   * 
   * @param x Foreign identifier to validate with
   * @return True if types are the same
   */
  virtual bool is_same_type(const basic_identifier &x) const override
  {
    return type_index() == x.type_index();
  }

  /**
   * Returns the undelying type_index
   * object of ids type
   * 
   * @return The type_index object
   */
  virtual const std::type_index &type_index() const override
  {
    return type_index_;
  }

  /**
   * Write the current id value to
   * the given stream and returns the
   * modified stream.
   * 
   * @param out Stream to write on
   * @return Modified stream
   */
  virtual std::ostream &print(std::ostream &out) const override
  {
    out << *id_;
    return out;
  }

  /**
   * Return the value of this identifier
   * 
   * @return The value of this identifier
   */
  operator T() const { return *id_; }

  /**
   * Clones this identifier
   *
   * @return A clone of this identifier
   */
  virtual basic_identifier *clone() const override
  {
    T id = *id_;
    return new self(id);
  }

  /**
   * Shares the value of this identifier with
   * a newly created identifier
   * 
   * @return New identifier with shared value
   */
  virtual self* share() override
  {
    std::unique_ptr<self> shared(new self());
    shared->id_ = id_;
    return shared.release();
  }

  /**
   * Isolate the shared value of the identifier
   * into a new yet unshared value
   */
  virtual void isolate() override
  {
    id_.reset(new T(*id_));
  }

  /**
   * Share identifiers value with given
   * foreign identifiers value.
   * 
   * If they are not of the same type false is returned
   * 
   * @param id Identifier to share the value with
   * @return True if sharing was successful.
   */
  virtual bool share_with(basic_identifier &id) override
  {
    if (!is_same_type(id)) {
      return false;
    }
    identifier<T> &xid = static_cast<identifier<T> &>(id);
    xid.id_ = id_;
    return true;
  }

  virtual bool is_valid() const override
  {
    return *id_ != 0;
  }

  /**
   * Return the value if the id
   *
   * @return The value
   */
  T value() const { return *id_; }

  /**
   * Set a new identifier value
   * 
   * @param val Value to set
   */
  void value(T val) { *id_ = val; }

  /**
   * Returns a reference to the value
   *
   * @return A reference to the value
   */
  T& reference() const { return *id_; }

private:
  std::shared_ptr<T> id_;
  static std::type_index type_index_;
};

template<typename T>
std::type_index identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>::type_index_ = std::type_index(
    typeid(identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>));


/**
 * @brief Identifier class for string
 */
template<>
class OOS_API identifier<std::string> : public basic_identifier
{
public:
  typedef identifier<std::string> self;  /**< Shortcut to self */

  /**
   * @brief Create an identifier
   */
  identifier() : id_(new std::string(""))
  { };

  /**
   * @brief Create an identifier with given string value
   * 
   * @param val String value of the identifier
   */
  explicit identifier(const std::string &val) : id_(new std::string(val))
  { }

  /**
   * @brief Copy assigns a new identifier from given string value
   * @param val String value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(const std::string &val)
  {
    *id_ = val;
    return *this;
  }

  virtual ~identifier()
  { };

  /**
   * Serialize the identifier value with
   * the given serializer.
   * 
   * @param id Name of the identifier
   * @param s The serializer to serialize with
   */
  virtual void serialize(const char *id, serializer &s) override
  {
    s.serialize(id, *id_);
  }

  /**
   * Returns true if own id value is less
   * than foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is less than foreign id value
   */
  virtual bool less(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return *id_ < static_cast<const self &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  /**
   * Returns true if own id value is equal to
   * foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is equal to foreign id value
   */
  virtual bool equal_to(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return *id_ == static_cast<const identifier<std::string> &>(x).value();
    } else {
      throw std::logic_error("not the same type");
    }
  }

  /**
   * Create a hash value of current
   * id value
   * 
   * @return The calculated hash value
   */
  virtual size_t hash() const override
  {
    std::hash<std::string> pk_hash;
    return pk_hash(*id_);
  }

  /**
   * Returns true if foreign id type is
   * the same as this id type
   * 
   * @param x Foreign identifier to validate with
   * @return True if types are the same
   */
  virtual bool is_same_type(const basic_identifier &x) const override
  {
    return type_index() == x.type_index();
  }

  /**
   * Returns the undelying type_index
   * object of ids type
   * 
   * @return The type_index object
   */
  virtual const std::type_index &type_index() const override
  {
    return type_index_;
  }

  /**
   * Write the current id value to
   * the given stream and returns the
   * modified stream.
   * 
   * @param out Stream to write on
   * @return Modified stream
   */
  virtual std::ostream &print(std::ostream &out) const override
  {
    out << *id_;
    return out;
  }

  /**
   * Return the value of this identifier
   * 
   * @return The value of this identifier
   */
  operator std::string() const { return *id_; }

  /**
   * Clones this identifier
   *
   * @return A clone of this identifier
   */
  virtual basic_identifier *clone() const override
  {
    return new self(*id_);
  }

  /**
   * Shares the value of this identifier with
   * a newly created identifier
   * 
   * @return New identifier with shared value
   */
  virtual identifier<std::string>* share() override
  {
    std::unique_ptr<identifier<std::string>> shared(new identifier<std::string>);
    shared->id_ = id_;
    return shared.release();
  }

  /**
   * Isolate the shared value of the identifier
   * into a new yet unshared value
   */
  virtual void isolate() override
  {
    id_.reset(new std::string(*id_));
  }

  /**
   * Share identifiers value with given
   * foreign identifiers value.
   * 
   * If they are not of the same type false is returned
   * 
   * @param id Identifier to share the value with
   * @return True if sharing was successful.
   */
  virtual bool share_with(basic_identifier &id) override
  {
    if (!is_same_type(id)) {
      return false;
    }
    identifier<std::string> &xid = static_cast<identifier<std::string> &>(id);
    xid.id_ = id_;
    return true;
  }

  virtual bool is_valid() const  override
  {
    return !id_->empty();
  }

  /**
   * Return the string value if the id
   *
   * @return The string value
   */
  std::string value() const { return *id_; }
  
  /**
   * Set a new identifier value
   * 
   * @param val Value to set
   */
  void value(const std::string &val) { *id_ = val; }

  /**
   * Returns a reference to the value
   *
   * @return A reference to the value
   */
  std::string& reference() const { return *id_; }

private:
  std::shared_ptr<std::string> id_;

  static std::type_index type_index_;
};

/**
 * @brief Shortcut to create a new identifier from value
 *
 * @tparam T Type of the identifier value
 * @param id The value of the identifier
 * @return Pointer to the new identifier object
 */
template<class T>
identifier<T> *make_id(const T &id)
{
  return new identifier<T>(id);
}

}

#endif /* IDENTIFIER_HPP */
