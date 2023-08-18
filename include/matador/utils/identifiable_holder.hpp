#ifndef OOS_IDENTIFIABLE_HOLDER_HPP
#define OOS_IDENTIFIABLE_HOLDER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/identifier.hpp"

#include <memory>

namespace matador {

/**
 * Base class for all pointer object
 * which can contain an identifiable
 */
class identifiable_holder
{
public:
  virtual ~identifiable_holder() = default;

  /**
   * Resets the object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  virtual void reset(const identifier &id) = 0;

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
  virtual const identifier& primary_key() const = 0;
  virtual identifier& primary_key() = 0;

  /**
   * Creates a new identifier object.
   *
   * @return Returns a new identifier object.
   */
  virtual identifier create_identifier() const = 0;
};

}
#endif //OOS_IDENTIFIABLE_HOLDER_HPP
