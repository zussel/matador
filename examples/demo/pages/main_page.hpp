#ifndef MATADOR_MAIN_PAGE_HPP
#define MATADOR_MAIN_PAGE_HPP

#include "matador/orm/persistence.hpp"

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

namespace matador {
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

private:
  matador::http::response get_index(const matador::http::request &p);
  matador::http::response get_movie(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;

  std::shared_ptr<matador::http::detail::template_part> index_template_;
  std::shared_ptr<matador::http::detail::template_part> details_template_;
};


#endif //MATADOR_MAIN_PAGE_HPP
