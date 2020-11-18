#ifndef MATADOR_REQUEST_HEADER_HPP
#define MATADOR_REQUEST_HEADER_HPP

namespace matador {
namespace http {

class request_header {
public:
  static constexpr const char* ACCEPT               = "Accept";
  static constexpr const char* ACCEPT_CHARSET       = "Accept-Charset";
  static constexpr const char* ACCEPT_ENCODING      = "Accept-Encoding";
  static constexpr const char* ACCEPT_LANGUAGE      = "Accept-Language";
  static constexpr const char* AUTHORIZATION        = "Authorization";
  static constexpr const char* CACHE_CONTROL        = "Cache-Control";
  static constexpr const char* CONNECTION           = "Connection";
  static constexpr const char* COOKIE               = "Cookie";
  static constexpr const char* CONTENT_LENGTH       = "Content-Length";
  static constexpr const char* CONTENT_MD5          = "Content-MD5";
  static constexpr const char* CONTENT_TYPE         = "Content-Type";
  static constexpr const char* DATE                 = "Date";
  static constexpr const char* EXPECT               = "Expect";
  static constexpr const char* FROM                 = "From";
  static constexpr const char* HOST                 = "Host";
  static constexpr const char* IF_MATCH             = "If-Match";
  static constexpr const char* IF_MODIFIED_SINCE    = "If-Modified-Since";
  static constexpr const char* IF_NONE_MATCH        = "If-None-Match";
  static constexpr const char* IF_RANGE             = "If-Range";
  static constexpr const char* IF_UNMODIFIED_SINCE  = "If-Unmodified-Since";
  static constexpr const char* MAX_FORWARDS         = "Max-Forwards";
  static constexpr const char* PRAGMA               = "Pragma";
  static constexpr const char* PROXY_AUTHORIZATION  = "Proxy-Authorization";
  static constexpr const char* RANGE                = "Range";
  static constexpr const char* REFERER              = "Referer";
  static constexpr const char* TE                   = "TE";
  static constexpr const char* TRANSFER_ENCODING    = "Transfer-Encoding";
  static constexpr const char* UPGRADE              = "Upgrade";
  static constexpr const char* USER_AGENT           = "User-Agent";
  static constexpr const char* VIA                  = "Via";
  static constexpr const char* WARNING              = "Warning";
};

}
}

#endif //MATADOR_REQUEST_HEADER_HPP
