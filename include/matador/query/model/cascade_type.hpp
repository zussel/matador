#ifndef MATADOR_CASCADE_TYPE_HPP
#define MATADOR_CASCADE_TYPE_HPP

#include "matador/utils/flags.hpp"

namespace matador::query {
/**
 * @brief Cascade types for database actions
 */
enum class cascade_type : uint8_t {
  None = 0,                        /**< Cascade type none */
  Remove = 1,                      /**< Cascade type remove */
  Update = 2,                      /**< Cascade type update */
  Insert = 4,                      /**< Cascade type insert */
  All = Remove | Update | Insert   /**< Cascade type all */
};

using cascade_types = utils::flags<cascade_type>;
}
#endif //MATADOR_CASCADE_TYPE_HPP
