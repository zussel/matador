#include "movie_page.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/template_engine.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/object/json_object_mapper.hpp"

#include "matador/orm/session.hpp"

#include "../models/movie.hpp"

using namespace matador;
using namespace matador::http;

movie_page::movie_page(matador::http::server &server, matador::persistence &p)
  : crud_page<movie>({"Movie", "Movies", "movie"}, server, p)
{}

void movie_page::prepare_json_form_data(json &data)
{
  using namespace matador;
  using namespace matador::http;

  session s(persistence_unit());

  auto directors = s.select<person>();

  json_object_mapper mapper;

  data["directors"] = mapper.to_json(directors);
}
