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
   * Gets the primary key of the foreign serializable
   *
   * @return The primary key of the foreign serializable
   */
  virtual std::shared_ptr<basic_identifier> primary_key() const = 0;
};

}
#endif //OOS_IDENTIFIABLE_HOLDER_HPP
