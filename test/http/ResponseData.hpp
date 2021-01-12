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
};

#endif //MATADOR_RESPONSEDATA_HPP
