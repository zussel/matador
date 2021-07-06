#ifndef MATADOR_REQUESTDATA_HPP
#define MATADOR_REQUESTDATA_HPP

class RequestData
{
public:
  static constexpr const char* GET_COMMON = \
    "GET /api HTTP/1.1\r\n" \
    "Host: de.wikipedia.org\r\n" \
    "From: frog@jmarshall.com\r\n" \
    "User-Agent: HTTPTool/1.0\r\n" \
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n";

  static constexpr const char* GET_COMMON_QUERY = \
    "GET /api?field=a%3D%3D3%26%26c%3C%3D9&orderby=name#fragment HTTP/1.1\r\n" \
    "Host: de.wikipedia.org\r\n" \
    "From: frog@jmarshall.com\r\n" \
    "User-Agent: HTTPTool/1.0\r\n" \
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n";

  static constexpr const char* GET_SHORT = \
    "GET /api HTTP/1.1\r\n" \
    "Host: de.wikipedia.org\r\n\r\n";

  static constexpr const char* POST_QUERY_BODY = \
    "POST /enlighten/rest HTTP/1.1\r\n" \
    "Host: api.opencalais.com\r\n" \
    "Content-Type: application/x-www-form-urlencoded\r\n" \
    "Content-Length: 32\r\n" \
    "\r\n" \
    "home=Cosby&favorite+flavor=flies";

  static constexpr const char* POST_XML_BODY = \
    "POST /experiments HTTP/1.1\r\n" \
    "Host: api.bonfire-project.eu:444\r\n" \
    "Accept: */*\r\n" \
    "Authorization: Basic XXX   \r\n" \
    "Accept-Encoding: gzip, deflate\r\n" \
    "Content-Type: application/vnd.bonfire+xml;\r\n" \
    "              charset=utf-8\r\n" \
    "Content-Length: 254\r\n" \
    "\r\n" \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" \
    "  <experiment xmlns=\"http://api.bonfire-project.eu/doc/schemas/coci\">\r\n" \
    "    <name>Scenario1</name>\r\n" \
    "    <walltime>1800</walltime>\r\n" \
    "    <description>Demo of scenario1 using Restfully</description>\r\n" \
    "   </experiment>\r\n";

  static constexpr const char* POST_FORM_DATA = \
    "POST /experiments HTTP/1.1\r\n" \
    "Host: api.bonfire-project.eu:444\r\n" \
    "Accept: */*\r\n" \
    "Authorization: Basic XXX   \r\n" \
    "Accept-Encoding: gzip, deflate\r\n" \
    "Content-Type: application/x-www-form-urlencoded\r\n" \
    "Content-Length: 32\r\n" \
    "\r\n" \
    "username=carl&password=secret123";

  static constexpr const char* POST_PARTIAL_BEGIN = \
    "POST /experiments HTTP/1.1\r\n" \
    "Host: api.bonfire-project.eu:444\r\n" \
    "Acce";

  static constexpr const char* POST_PARTIAL_END = \
    "pt: */*\r\n" \
    "Authorization: Basic XXX\r\n" \
    "Accept-Encoding: gzip, deflate\r\n" \
    "Content-Type: application/vnd.bonfire+xml; charset=utf-8\r\n" \
    "Content-Length: 254\r\n" \
    "\r\n" \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" \
    "  <experiment xmlns=\"http://api.bonfire-project.eu/doc/schemas/occi\">\r\n" \
    "    <name>Scenario1</name>\r\n" \
    "    <walltime>1800</walltime>\r\n" \
    "    <description>Demo of scenario1 using Restfully</description>\r\n" \
    "   </experiment>\r\n";

  static constexpr const char* POST_PARTIAL_BEGIN_2 = \
    "POST /experiments HTTP/1.1\r\n" \
    "Host: api.bonfire-project.eu:444\r\n" \
    "Acce";

  static constexpr const char* POST_PARTIAL_MIDDLE_2 = \
    "pt: */*\r\n" \
    "Authorization: Basic XXX\r\n" \
    "Accept-Encoding: gzip, deflate\r\n" \
    "Content-Type: application/vnd.bonfire+xml; charset=utf-8\r\n" \
    "Content-Length: 254\r\n" \
    "\r\n" \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" \
    "  <experiment xmlns=\"http://api.bonfire-project.eu/doc/schemas/occi\">\r\n";

  static constexpr const char* POST_PARTIAL_END_2 = \
    "    <name>Scenario1</name>\r\n" \
    "    <walltime>1800</walltime>\r\n" \
    "    <description>Demo of scenario1 using Restfully</description>\r\n" \
    "   </experiment>\r\n";

};
#endif //MATADOR_REQUESTDATA_HPP
