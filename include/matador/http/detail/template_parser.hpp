#ifndef MATADOR_TEMPLATE_PARSER_HPP
#define MATADOR_TEMPLATE_PARSER_HPP

#include <string>

namespace matador {

class string_cursor;

namespace http {
namespace detail {

std::string parse_token(string_cursor &cursor);

}
}
}

#endif //MATADOR_TEMPLATE_PARSER_HPP
