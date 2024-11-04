#ifndef OOS_DATABASE_TYPES_HPP
#define OOS_DATABASE_TYPES_HPP

#include <cstdint>
#include <string>

namespace matador {

/**
 * @brief Enumeration type of all supported builtin data types
 */
enum class data_type : uint8_t {
  type_char = 0,           /*!< Data type char */
  type_short,              /*!< Data type short */
  type_int,                /*!< Data type int */
  type_long,               /*!< Data type long */
  type_long_long,          /*!< Data type long long */
  type_unsigned_char,      /*!< Data type unsigned char */
  type_unsigned_short,     /*!< Data type unsigned short */
  type_unsigned_int,       /*!< Data type unsigned int */
  type_unsigned_long,      /*!< Data type unsigned long */
  type_unsigned_long_long, /*!< Data type unsigned long long */
  type_float,              /*!< Data type float */
  type_double,             /*!< Data type double */
  type_bool,               /*!< Data type bool */
  type_char_pointer,       /*!< Data type character pointer */
  type_varchar,            /*!< Data type varchar */
  type_text,               /*!< Data type text */
  type_date,               /*!< Data type date */
  type_time,               /*!< Data type time */
  type_blob,               /*!< Data type blob */
  type_null,               /*!< Data type null */
  type_unknown             /*!< Data type unknown */
};

}

#endif /* OOS_DATABASE_TYPES_HPP */
