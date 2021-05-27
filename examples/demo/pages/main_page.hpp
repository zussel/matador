#ifndef MATADOR_MAIN_PAGE_HPP
#define MATADOR_MAIN_PAGE_HPP

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

namespace matador {
class persistence;
namespace http {
namespace detail {
class template_part;
}
class server;
class request;
}
}

class main_page
{
public:
  main_page(matador::http::server &s, matador::persistence &p);

  matador::http::response view(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;

  std::shared_ptr<matador::http::detail::template_part> index_template_;
};


#endif //MATADOR_MAIN_PAGE_HPP
