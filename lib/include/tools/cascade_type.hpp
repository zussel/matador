//
// Created by sascha on 12/16/15.
//

#ifndef OOS_CASCADE_TYPE_HPP
#define OOS_CASCADE_TYPE_HPP

namespace oos {

/**
 * @brief Cascade types for database actions
 */
enum cascade_type
{
  NONE = 0,                        /**< Cascade type none */
  REMOVE = 1,                      /**< Cascade type remove */
  UPDATE = 2,                      /**< Cascade type update */
  INSERT = 4,                      /**< Cascade type insert */
  ALL = REMOVE | UPDATE | INSERT   /**< Cascade type all */
};

}
#endif //OOS_CASCADE_TYPE_HPP
