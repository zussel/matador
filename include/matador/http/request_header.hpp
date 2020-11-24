#ifndef MATADOR_REQUEST_HEADER_HPP
#define MATADOR_REQUEST_HEADER_HPP

namespace matador {
namespace http {

class request_header {
public:
  static const char* ACCEPT;
  static const char* ACCEPT_CHARSET;
  static const char* ACCEPT_ENCODING;
  static const char* ACCEPT_LANGUAGE;
  static const char* AUTHORIZATION;
  static const char* CACHE_CONTROL;
  static const char* CONNECTION;
  static const char* COOKIE;
  static const char* CONTENT_LENGTH;
  static const char* CONTENT_MD5;
  static const char* CONTENT_TYPE;
  static const char* DATE;
  static const char* EXPECT;
  static const char* FROM;
  static const char* HOST;
  static const char* IF_MATCH;
  static const char* IF_MODIFIED_SINCE;
  static const char* IF_NONE_MATCH;
  static const char* IF_RANGE;
  static const char* IF_UNMODIFIED_SINCE;
  static const char* MAX_FORWARDS;
  static const char* PRAGMA;
  static const char* PROXY_AUTHORIZATION;
  static const char* RANGE;
  static const char* REFERER;
  static const char* TE;
  static const char* TRANSFER_ENCODING;
  static const char* UPGRADE;
  static const char* USER_AGENT;
  static const char* VIA;
  static const char* WARNING;
};

}
}

#endif //MATADOR_REQUEST_HEADER_HPP
