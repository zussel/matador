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

#include "object/object_atomizer.hpp"

#include <typeindex>
#include <iosfwd>
#include <stdexcept>

namespace oos {

/// @cond OOS_DEV

template < typename T, class Enable = void >
class identifier;

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
   * Serialize the identifier.
   *
   * @param id Id of the identifier.
   * @param writer The object to write to
   */
  virtual void serialize(const char *id, object_writer &writer) const = 0;

  /**
   * Deserialize the identifier.
   *
   * @param id Id of the identifier.
   * @param reader The object to read from
   */
  virtual void deserialize(const char *id, object_reader &reader) = 0;

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

  virtual std::ostream &print(std::ostream &out) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const basic_identifier &x);

  virtual bool is_valid() const = 0;

  template<class T>
  T id() const;

protected:
  /**
   * The returns the type_index of a specific type
   *
   * @return The specific type_index
   */
  virtual const std::type_index &type_index() const = 0;

private:
  template < typename T, class Enable >
  friend class identifier;

};

template<class T>
T basic_identifier::id() const
{
  if (type_index() == std::type_index(typeid(identifier < T > ))) {
    return static_cast<const identifier <T> &>(*this).value();
  } else {
    throw std::logic_error("not the same type");
  }
}

}

#endif //OOS_BASIC_IDENTIFIER_HPP
