#include "main_page.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/template_engine.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/object/json_object_mapper.hpp"

#include "matador/orm/session.hpp"

#include "../models/movie.hpp"

using namespace matador;
using namespace matador::http;

main_page::main_page(matador::http::server &server, matador::persistence &p)
  : log_(matador::create_logger("MainPage"))
  , persistence_(p)
{
  file f("../templates/movies.matador", "r");
  auto tmpl = read_as_text(f);
  f.close();

  index_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_details.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  details_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_create.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  create_template_ = template_engine::build(tmpl);

  server.on_get("/", [this](const request &req) {
    return get_index(req);
  });

  server.on_get("/movie/{id: \\d+}", [this](const request &req) {
    return get_movie(req);
  });

  server.on_get("/movie/create", [this](const request &req) {
    return create_movie(req);
  });
}

matador::http::response main_page::get_index(const request &)
{
  json data;
  data["title"] = "Movies";

  session s(persistence_);

  auto movies = s.select<movie>();

  json_object_mapper mapper;

  data["movies"] = mapper.to_json(movies);

  log_.info(mapper.to_string(movies));

  return response::ok(template_engine::render(index_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response main_page::get_movie(const request &p)
{
  json data;
  data["title"] = "Movie";

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto movies = s.select<movie>();
  auto it = movies.find_if([id](const auto &m) {
    return m->id == id;
  });

  if (it == movies.end()) {
    return response::no_content();

  }

  json_object_mapper mapper;

  data["title"] = std::string("Movie: ") + (*it)->title;
  data["movie"] = mapper.to_json(*it);

  return response::ok(template_engine::render(details_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response main_page::create_movie(const request &p)
{
  json data;
  data["title"] = "Create Movie";

  return response::ok(template_engine::render(create_template_, data), mime_types::TYPE_TEXT_HTML);
}
