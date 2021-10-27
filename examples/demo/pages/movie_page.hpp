#ifndef MATADOR_MOVIE_PAGE_HPP
#define MATADOR_MOVIE_PAGE_HPP

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

class movie_page
{
public:
  movie_page(matador::http::server &s, matador::persistence &p);

private:
  matador::http::response list(const matador::http::request &);
  matador::http::response view(const matador::http::request &p);
  matador::http::response create(const matador::http::request &);
  matador::http::response edit(const matador::http::request &p);
  matador::http::response remove(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;

  std::shared_ptr<matador::http::detail::template_part> list_template_;
  std::shared_ptr<matador::http::detail::template_part> details_template_;
  std::shared_ptr<matador::http::detail::template_part> create_template_;
  std::shared_ptr<matador::http::detail::template_part> edit_template_;
  std::shared_ptr<matador::http::detail::template_part> delete_template_;
};


#endif //MATADOR_MOVIE_PAGE_HPP
