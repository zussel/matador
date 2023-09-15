#include "matador/db/mssql/mssql_types.hpp"

namespace matador {

namespace mssql {

int type2int(data_type type)
{
  switch(type) {
    case data_type::type_char:
      return SQL_C_SSHORT;
    case data_type::type_short:
      return SQL_C_SSHORT;
    case data_type::type_int:
      return SQL_C_SLONG;
    case data_type::type_long:
#ifdef __LP64__
      return SQL_C_SBIGINT;
#else
      return SQL_C_SLONG;
#endif
    case data_type::type_long_long:
      return SQL_C_SBIGINT;
    case data_type::type_unsigned_char:
      return SQL_C_SHORT;
    case data_type::type_unsigned_short:
      return SQL_C_USHORT;
    case data_type::type_unsigned_int:
      return SQL_C_ULONG;
    case data_type::type_unsigned_long:
#ifdef __LP64__
      return SQL_C_UBIGINT;
#else
      return SQL_C_ULONG;
#endif
    case data_type::type_unsigned_long_long:
      return SQL_C_UBIGINT;
    case data_type::type_bool:
      return SQL_C_BIT;
    case data_type::type_float:
      return SQL_C_FLOAT;
    case data_type::type_double:
      return SQL_C_DOUBLE;
    case data_type::type_char_pointer:
      return SQL_C_CHAR;
    case data_type::type_varchar:
      return SQL_C_CHAR;
    case data_type::type_text:
      return SQL_C_CHAR;
    case data_type::type_date:
      return SQL_C_TYPE_DATE;
    case data_type::type_time:
      return SQL_C_TYPE_TIMESTAMP;
    default:
    {
      throw std::logic_error("mssql statement: unknown type");
    }
  }
}

}

}