#ifndef MATADOR_HTTP_HPP
#define MATADOR_HTTP_HPP

namespace matador {
namespace http {

class http
{
public:
  enum method_t {
    UNKNOWN,
    GET,
    POST,
    PUT,
    DELETE,
    OPTIONS,
    HEAD
  };
};

const char* to_string(http::method_t method);

}
}

#endif //MATADOR_HTTP_HPP
