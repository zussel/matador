#include "matador/http/response.hpp"
#include "matador/http/response_header.hpp"

using namespace std;

namespace matador {
namespace http {

std::string response::to_string() const
{
  std::string result(http::to_string(status));
  for(const auto &p : headers) {
    result += p.first + ": " + p.second + "\r\n";
  }

  result += response_header::CONTENT_LENGTH + std::string(": ") + ::to_string(content_type.length) + "\r\n";
  result += response_header::CONTENT_TYPE + std::string(": ") + content_type.type + ";\r\n\r\n";
//  result += response_header::CONTENT_LANGUAGE + std::string(": ") + content_type.language + "\r\n\r\n";

  result += body + "\r\n";

  return result;
}

}
}