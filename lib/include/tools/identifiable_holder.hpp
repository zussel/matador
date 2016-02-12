//
// Created by sascha on 2/11/16.
//

#ifndef OOS_IDENTIFIABLE_HOLDER_HPP
#define OOS_IDENTIFIABLE_HOLDER_HPP

#include <memory>

namespace oos {

class basic_identifier;

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
