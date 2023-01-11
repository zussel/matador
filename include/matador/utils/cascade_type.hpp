#ifndef OOS_CASCADE_TYPE_HPP
#define OOS_CASCADE_TYPE_HPP

namespace matador {

/**
 * @brief Cascade types for database actions
 */
enum class cascade_type
{
  NONE = 0,                        /**< Cascade type none */
  REMOVE = 1,                      /**< Cascade type remove */
  UPDATE = 2,                      /**< Cascade type update */
  INSERT = 4,                      /**< Cascade type insert */
  ALL = REMOVE | UPDATE | INSERT   /**< Cascade type all */
};

}
#endif //OOS_CASCADE_TYPE_HPP
