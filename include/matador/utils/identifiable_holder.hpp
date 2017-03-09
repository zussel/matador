//
// Created by sascha on 2/11/16.
//

#ifndef OOS_IDENTIFIABLE_HOLDER_HPP
#define OOS_IDENTIFIABLE_HOLDER_HPP

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

#include <memory>

namespace matador {

class basic_identifier;

/**
 * Base class for all pointer object
 * which can contain an identifiable
 */
class identifiable_holder
{
public:
  virtual ~identifiable_holder() {}

  /**
   * Resets the object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  virtual void reset(const std::shared_ptr<basic_identifier> &id) = 0;

  /**
   * Returns true if serializable has a primary key
   *
   * @return true if serializable has a primary key
   */
  virtual bool has_primary_key() const = 0;

  /**
   * Gets the primary key of the foreign serializable
   *
   * @return The primary key of the foreign serializable
   */
  virtual std::shared_ptr<basic_identifier> primary_key() const = 0;

  /**
   * Creates a new identifier object.
   *
   * @return Returns a new identifier object.
   */
  virtual basic_identifier* create_identifier() const = 0;

};

}
#endif //OOS_IDENTIFIABLE_HOLDER_HPP
