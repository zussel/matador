#include "matador/utils/types.hpp"

namespace matador::utils {

void initialize_by_data_type(data_type type, any_type &val)
{
  switch (type) {
    case data_type::type_char:
      val.emplace<char>();
      break;
    case data_type::type_short:
      val.emplace<short>();
      break;
    case data_type::type_int:
      val.emplace<int>();
      break;
    case data_type::type_long:
      val.emplace<long>();
      break;
    case data_type::type_long_long:
      val.emplace<long long>();
      break;
    case data_type::type_unsigned_char:
      val.emplace<unsigned char>();
      break;
    case data_type::type_unsigned_short:
      val.emplace<unsigned short>();
      break;
    case data_type::type_unsigned_int:
      val.emplace<unsigned int>();
      break;
    case data_type::type_unsigned_long:
      val.emplace<unsigned long>();
      break;
    case data_type::type_unsigned_long_long:
      val.emplace<unsigned long long>();
      break;
    case data_type::type_bool:
      val.emplace<bool>();
      break;
    case data_type::type_float:
      val.emplace<float>();
      break;
    case data_type::type_double:
      val.emplace<double>();
      break;
    case data_type::type_char_pointer:
      val.emplace<const char*>();
      break;
    case data_type::type_varchar:
    case data_type::type_text:
      val.emplace<std::string>();
      break;
    case data_type::type_date:
      val.emplace<date>();
      break;
    case data_type::type_time:
      val.emplace<time>();
      break;
    case data_type::type_blob:
      val.emplace<utils::blob>();
      break;
    default:
      val.emplace<nullptr_t>();
  }

}

}