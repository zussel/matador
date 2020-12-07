#ifndef MATADOR_JSON_SERIALIZER_HPP
#define MATADOR_JSON_SERIALIZER_HPP

#include "matador/utils/access.hpp"

#include <sstream>

namespace matador {

class json_serializer
{
public:

  template < class T >
  std::string to_string(const T &obj)
  {
    json_ << "{\n";

    matador::access::serialize(*this, obj);

    json_ << "}\n";

    return json_.str();
  }

  // numbers
  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<T>::value && std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    json_ << "  \"" << id << "\": " << val << "\n";
  }

  // boolean
  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<T>::value && std::is_same<V, bool>::value>::type* = 0)
  {
    json_ << "  \"" << id << "\": " << std::boolalpha << val << "\n";
  }

  void serialize(const char *id, std::string &val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
  {
    json_ << "  \"" << id << "\": \"" << val << "\"\n";
  }

private:
  std::stringstream json_;
};

}
#endif //MATADOR_JSON_SERIALIZER_HPP
