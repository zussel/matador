#ifndef MATADOR_RESPONSEDATA_HPP
#define MATADOR_RESPONSEDATA_HPP

class ResponseData
{
public:
  static constexpr const char* EMPTY_BODY = \
    "HTTP/1.1 204 No Content\r\n" \
    "Server: Matador\r\n\r\n";

  static constexpr const char* FILLED_BODY = \
    "HTTP/1.1 200 OK\r\n" \
    "Server: Matador\r\n" \
    "Content-Type: application/json\r\n" \
    "Content-Length: 31\r\n" \
    "\r\n" \
    "{ \"name\": \"George\", \"age\": 37 }";

  static constexpr const char* FILLED_PARTIAL_BODY_BEGIN = \
    "HTTP/1.1 200 OK\r\n" \
    "Server: Matador\r\n" \
    "Content-Type: application/json\r\n" \
    "Content-Length: 31\r\n" \
    "\r\n" \
    "{ \"name\": \"Geo";

  static constexpr const char* FILLED_PARTIAL_BODY_END = \
    R"(rge", "age": 37 })";

  static constexpr const char* FILLED_PARTIAL_BODY_BEGIN_2 = \
    "HTTP/1.1 200 OK\r\n" \
    "Server: Matador\r\n" \
    "Content-Type: application/json\r\n" \
    "Content-Length: 31";

  static constexpr const char* FILLED_PARTIAL_BODY_MIDDLE_2 = \
    "\r\n" \
    "\r\n" \
    "{ \"name\": \"Geo";

  static constexpr const char* FILLED_PARTIAL_BODY_END_2 = \
    R"(rge", "age": 37 })";
};

#endif //MATADOR_RESPONSEDATA_HPP
