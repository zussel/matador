#ifndef OOS_DATABASE_TYPES_HPP
#define OOS_DATABASE_TYPES_HPP

#include <cstdint>

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

enum class basic_type : uint8_t {
  type_int8 = 0,
  type_int16,
  type_int32,
  type_int64,
  type_uint8,
  type_uint16,
  type_uint32,
  type_uint64,
  type_float,
  type_double,
  type_varchar,
  type_text,
  type_blob,
  type_timestamp,
  type_date
};

}

#endif /* OOS_DATABASE_TYPES_HPP */
