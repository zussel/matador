#ifndef MATADOR_MOVIE_PAGE_HPP
#define MATADOR_MOVIE_PAGE_HPP

#include "crud_page.hpp"

#include "../models/movie.hpp"

#include "matador/orm/persistence.hpp"

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

class movie_page : public crud_page<movie>
{
public:
  movie_page(matador::http::server &s, matador::persistence &p);

protected:
  void prepare_json_form_data(matador::json &data) override;
};


#endif //MATADOR_MOVIE_PAGE_HPP
