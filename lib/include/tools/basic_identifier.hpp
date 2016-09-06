//
// Created by sascha on 25.09.15.
//

#ifndef OOS_BASIC_IDENTIFIER_HPP
#define OOS_BASIC_IDENTIFIER_HPP

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

#include <typeindex>
#include <iosfwd>
#include <stdexcept>
#include <object/basic_identifier_serializer.hpp>

namespace oos {

/// @cond OOS_DEV

template < typename T, class Enable = void >
class identifier;

class serializer;

/// @endcond

/**
 * This is the base class for any identifier class
 * It supports interfaces for comparing, sharing
 * and cloning
 * It also implements the output operator to streams
 * and a generic method to retrieve the value of the
 * concrete identifier.
 */
class OOS_API basic_identifier
{
public:
  basic_identifier();
  virtual ~basic_identifier();

  /**
   * Checks if two identifiers are equal
   *
   * @param x The identifier to compare with
   * @return True if identifiers are equal
   */
  bool operator==(const basic_identifier &x) const;

  /**
   * Checks if two identifiers are not equal
   *
   * @param x The identifier to compare with
   * @return True if identifiers are not equal
   */
  bool operator!=(const basic_identifier &x) const;

  /**
   * Checks if this identifier is less than another identifier.
   *
   * @param x The identifier to compare with
   * @return True this identifier is less than another identifier
   */
  bool operator<(const basic_identifier &x) const;

  /**
   * Interface for the less than operator
   *
   * @param x The identifier to compare to
   * @return True if this identifier is less than another identifier
   */
  virtual bool less(const basic_identifier &x) const = 0;

  /**
   * Interface for the equal to operator
   *
   * @param x The identifier to compare to
   * @return True if both identifiers are equal
   */
  virtual bool equal_to(const basic_identifier &x) const = 0;

  virtual void serialize(const char*, serializer &) = 0;
  /**
   * Returns a unique hash number
   *
   * @return A unique hash number
   */
  virtual size_t hash() const = 0;

  /**
   * Returns true if the given identifier
   * is of the same type as this identifier
   *
   * @param x Identifier to compare the type with
   * @return True if their types are equal
   */
  virtual bool is_same_type(const basic_identifier &x) const = 0;

  /**
   * Clone this identifier
   *
   * @return The cloned identifier
   */
  virtual basic_identifier *clone() const = 0;

  /**
   * Share the id of the identifier. The underlying
   * concrete id of an identifier is a shared object
   * so it can be shared within more identifiers
   * of the same type
   *
   * @return A new identifier object with the shared id.
   */
  virtual basic_identifier* share() = 0;

  /**
   * Isolate means unshring a shared identifier id creating
   * a new copy of the identifier id.
   */
  virtual void isolate() = 0;

  /**
   * Share identifiers id with given identifier.
   *
   * @param id The identifier to share the id with.
   */
  virtual void share_with(basic_identifier &id) = 0;

  /**
   * Prints the identifier to the given stream.
   *
   * @param out The stream to write to
   */
  virtual std::ostream &print(std::ostream &out) const = 0;

  /**
   * Writes the given identifier to the given ostream.
   *
   * @param os The ostream to write to.
   * @param x The identifier to write.
   * @return Reference to the ostream.
   */
  friend OOS_API std::ostream &operator<<(std::ostream &os, const basic_identifier &x);

  /**
   * Returns true if the identifier has a
   * valid value.
   *
   * @return True id identifier is valid.
   */
  virtual bool is_valid() const = 0;

  /**
   * Cast the value of the concrete identifier
   * to the type T. Throws a logical_error if
   * a cast is not possible.
   *
   * @tparam T The type to cast to.
   * @return The casted value.
   */
//  template<class T>
//  T id() const;

  /**
   * Returns true if identifier should be
   * treated as plain value (default)
   *
   * @return True if treated as plain value
   */
  bool as_value() const;

  /**
   * Sets flag indicating that identifier
   * id treated as plain value (default)
   *
   * @param asvalue True if identifier should be treated as plain value.
   */
  void as_value(bool asvalue);

protected:
  friend class basic_identifier_serializer;
  /**
   * The returns the type_index of a specific type
   *
   * @return The specific type_index
   */
  virtual const std::type_index &type_index() const = 0;

private:
  template < typename T, class Enable >
  friend class identifier;

  bool as_value_ = false;
};

}

#endif //OOS_BASIC_IDENTIFIER_HPP
