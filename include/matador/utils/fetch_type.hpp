#ifndef MATADOR_FETCH_TYPE_HPP
#define MATADOR_FETCH_TYPE_HPP

#include <cstdint>

namespace matador {

/**
 * @brief Defines fetch types
 *
 * Defines fetch types for foreign relations
 */
enum class fetch_type : uint8_t
{
  LAZY,   /**< Indicates lazy fetch */
  EAGER   /**< Indicates eager fetch */
};

}

#endif //MATADOR_FETCH_TYPE_HPP
