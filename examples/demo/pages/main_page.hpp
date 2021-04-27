#ifndef MATADOR_MAIN_PAGE_HPP
#define MATADOR_MAIN_PAGE_HPP

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

namespace matador {
namespace http {

class server;
class request;

}
}

class main_page
{
public:
  explicit main_page(matador::http::server &s);



private:
  matador::logger log_;
};


#endif //MATADOR_MAIN_PAGE_HPP
