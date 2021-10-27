#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
#define OOS_UTILS_API __declspec(dllexport)
#define EXPIMP_UTILS_TEMPLATE
#else
#define OOS_UTILS_API __declspec(dllimport)
#define EXPIMP_UTILS_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/varchar.hpp"

#include <type_traits>
#include <stdexcept>
#include <functional>
#include <memory>

namespace matador {

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
  identifier() : id_(0) {}

  /**
   * @brief Create an identifier with given value
   * @param val Value of the identifier
   */
  identifier(T val) : id_(val) {}

  ~identifier() override = default;

  /**
   * @brief Default copy assignment constructor
   *
   * @return The copied identifier object
   */
  identifier& operator=(const identifier &x) {
    if (this != &x) {
      id_ = x.id_;
    }
    return *this;
  }

  /**
   * @brief Default move assignment constructor
   *
   * @return The moved identifier object
   */
  identifier& operator=(identifier &&x) noexcept {
    id_ = x.id_;
    return *this;
  }

  /**
   * @brief Default copy constructor
   */
  identifier(const identifier &) = default;

  /**
   * @brief Default move constructor
   */
  identifier(identifier &&) noexcept = default;

  /**
   * @brief Copy assigns a new identifier from given value
   * @param val Value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(T val)
  {
    id_ = val;
    return *this;
  }

  /**
   * Serialize the identifier value with
   * the given serializer.
   * 
   * @param id Name of the identifier
   * @param s The serializer to serialize with
   */
  void serialize(const char *id, serializer &s) override
  {
    s.serialize(id, id_);
  }

  /**
   * Returns true if own id value is less
   * than foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is less than foreign id value
   */
  bool less(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ < static_cast<const self &>(x).value();
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
  bool equal_to(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ == static_cast<const identifier<T> &>(x).value();
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
  size_t hash() const override
  {
    std::hash<T> pk_hash;
    return pk_hash(id_);
  }

  /**
   * Returns true if foreign id type is
   * the same as this id type
   * 
   * @param x Foreign identifier to validate with
   * @return True if types are the same
   */
  bool is_same_type(const basic_identifier &x) const override
  {
    return type_index() == x.type_index();
  }

  /**
   * Returns the underlying type_index
   * object of ids type
   * 
   * @return The type_index object
   */
  const std::type_index &type_index() const override
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
  std::ostream &print(std::ostream &out) const override
  {
    out << id_;
    return out;
  }

  /**
   * Return the value of this identifier
   * 
   * @return The value of this identifier
   */
  operator T() const { return id_; }

  /**
   * Clones this identifier
   *
   * @return A clone of this identifier
   */
  basic_identifier *clone() const override
  {
    return new self(id_);
  }

  /**
   * Returns true if identifier value is valid.
   *
   * @return True if identifier value is valid.
   */
  bool is_valid() const override
  {
    return id_ != 0;
  }

  /**
   * Return the value if the id
   *
   * @return The value
   */
  T value() const { return id_; }

  /**
   * Set a new identifier value
   * 
   * @param val Value to set
   */
  void value(T val) { id_ = val; }

  /**
   * Returns a reference to the value
   *
   * @return A reference to the value
   */
  T& reference() { return id_; }

private:
  T id_ = {};
  static std::type_index type_index_;
};

template<typename T>
std::type_index identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>::type_index_ = std::type_index(
    typeid(identifier<T, typename std::enable_if<std::is_integral<T>::value>::type>));


/**
 * @brief Identifier class for string
 */
template<>
class OOS_UTILS_API identifier<std::string> : public basic_identifier
{
public:
  typedef identifier<std::string> self;  /**< Shortcut to self */

  /**
   * @brief Create an identifier
   */
  identifier() : id_("")
  { };

  /**
   * @brief Create an identifier with given string value
   * 
   * @param val String value of the identifier
   */
  explicit identifier(std::string val) : id_(std::move(val))
  { }

  /**
   * Create an identifier from given string
   *
   * @param val Value of identifier
   */
  identifier(const char *val) :id_(val) {}

  /**
   * @brief Copy assigns a new identifier from given string value
   * @param val String value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(const std::string &val)
  {
    id_ = val;
    return *this;
  }

  /**
   * @brief Copy assigns a new identifier from given string value
   * @param val String value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(const char *val)
  {
    id_.assign(val);
    return *this;
  }

  ~identifier() override = default;

  /**
   * Serialize the identifier value with
   * the given serializer.
   * 
   * @param id Name of the identifier
   * @param s The serializer to serialize with
   */
  void serialize(const char *id, serializer &s) override
  {
    s.serialize(id, id_);
  }

  /**
   * Returns true if own id value is less
   * than foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is less than foreign id value
   */
  bool less(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ < static_cast<const self &>(x).value();
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
  bool equal_to(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ == static_cast<const identifier<std::string> &>(x).value();
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
  size_t hash() const override
  {
    std::hash<std::string> pk_hash;
    return pk_hash(id_);
  }

  /**
   * Returns true if foreign id type is
   * the same as this id type
   * 
   * @param x Foreign identifier to validate with
   * @return True if types are the same
   */
  bool is_same_type(const basic_identifier &x) const override
  {
    return type_index() == x.type_index();
  }

  /**
   * Returns the undelying type_index
   * object of ids type
   * 
   * @return The type_index object
   */
  const std::type_index &type_index() const override
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
  std::ostream &print(std::ostream &out) const override
  {
    out << id_;
    return out;
  }

  /**
   * Return the value of this identifier
   * 
   * @return The value of this identifier
   */
  operator std::string() const { return id_; }

  /**
   * Clones this identifier
   *
   * @return A clone of this identifier
   */
  basic_identifier *clone() const override
  {
    return new self(id_);
  }

  /**
   * Returns true if identifier value is valid.
   *
   * @return True if identifier value is valid.
   */
  bool is_valid() const  override
  {
    return !id_.empty();
  }

  /**
   * Return the string value if the id
   *
   * @return The string value
   */
  std::string value() const { return id_; }
  
  /**
   * Set a new identifier value
   * 
   * @param val Value to set
   */
  void value(const std::string &val) { id_ = val; }

  /**
   * Returns a reference to the value
   *
   * @return A reference to the value
   */
  const std::string& reference() const { return id_; }

private:
  std::string id_;

  static std::type_index type_index_;
};

/**
 * @brief Identifier class for varchar
 */
template < int SIZE, class T >
class OOS_UTILS_API identifier<varchar<SIZE, T>> : public basic_identifier
{
public:
  typedef varchar<SIZE, T> varchar_type;  /**< Shortcut to varchar type */
  typedef identifier<varchar_type> self;  /**< Shortcut to self */
  typedef typename varchar_type::value_type value_type; /**< Shortcut to varchar value type */

  /**
   * @brief Create an identifier
   */
  identifier() = default;

  /**
   * @brief Create an identifier with given string value
   *
   * @param val String value of the identifier
   */
  explicit identifier(const std::string& val) : id_(val)
  { }

  /**
   * @brief Create an identifier with given string value
   *
   * @param val String value of the identifier
   */
  explicit identifier(const varchar_type &val) : id_(val)
  { }

  /**
   * Create an identifier from given string
   *
   * @param val Value of identifier
   */
  identifier(const char *val) :id_(val) {}

  /**
   * @brief Copy assigns a new identifier from given string value
   * @param val String value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(const std::string &val)
  {
    id_ = val;
    return *this;
  }

  /**
   * @brief Copy assigns a new identifier from given string value
   * @param val String value to be assigned
   * @return Reference to the new identfifier
   */
  identifier& operator=(const char *val)
  {
    id_.assign(val);
    return *this;
  }

  ~identifier() override = default;

  /**
   * Serialize the identifier value with
   * the given serializer.
   *
   * @param id Name of the identifier
   * @param s The serializer to serialize with
   */
  void serialize(const char *id, serializer &s) override
  {
    s.serialize(id, id_.value(), SIZE);
  }

  /**
   * Returns true if own id value is less
   * than foreign id value.
   *
   * @param x Foreign id to compare
   * @return True if own id value is less than foreign id value
   */
  bool less(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ < static_cast<const self &>(x).id_;
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
  bool equal_to(const basic_identifier &x) const override
  {
    if (this->is_same_type(x)) {
      return id_ == static_cast<const identifier<varchar_type> &>(x).id_;
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
  size_t hash() const override
  {
    return id_.hash();
  }

  /**
   * Returns true if foreign id type is
   * the same as this id type
   *
   * @param x Foreign identifier to validate with
   * @return True if types are the same
   */
  bool is_same_type(const basic_identifier &x) const override
  {
    return type_index() == x.type_index();
  }

  /**
   * Returns the undelying type_index
   * object of ids type
   *
   * @return The type_index object
   */
  const std::type_index &type_index() const override
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
  std::ostream &print(std::ostream &out) const override
  {
    out << id_.value();
    return out;
  }

  /**
   * Return the value of this identifier
   *
   * @return The value of this identifier
   */
  operator value_type () const { return id_.value(); }

  /**
   * Clones this identifier
   *
   * @return A clone of this identifier
   */
  basic_identifier *clone() const override
  {
    return new self(id_);
  }

  /**
   * Returns true if identifier value is valid.
   *
   * @return True if identifier value is valid.
   */
  bool is_valid() const  override
  {
    return !id_.empty();
  }

  /**
   * Return the string value if the id
   *
   * @return The string value
   */
  value_type value() { return id_.value(); }

  /**
   * Return the string value if the id
   *
   * @return The string value
   */
  const value_type value() const { return id_.value(); }

  /**
   * Set a new identifier value
   *
   * @param val Value to set
   */
  void value(const std::string &val) { id_.assign(val); }

  /**
   * Set a new identifier value
   *
   * @param val Value to set
   */
  void value(const varchar_type &val) { id_ = val; }

  /**
   * Returns a reference to the value
   *
   * @return A reference to the value
   */
  const std::string& reference() const { return id_.value(); }

private:
  varchar_type id_;

  static std::type_index type_index_;
};

template < int SIZE, class T >
std::type_index identifier<varchar<SIZE, T>>::type_index_ = std::type_index(typeid(identifier<varchar<SIZE, T>>));

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
