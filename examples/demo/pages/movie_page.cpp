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
  : log_(matador::create_logger("MoviePage"))
  , persistence_(p)
{
  file f("../templates/movies.matador", "r");
  auto tmpl = read_as_text(f);
  f.close();

  list_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_details.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  details_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_create.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  create_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_edit.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  edit_template_ = template_engine::build(tmpl);

  f.open("../templates/movie_delete.matador", "r");
  tmpl = read_as_text(f);
  f.close();

  delete_template_ = template_engine::build(tmpl);

  server.on_get("/movie", [this](const request &req) {
    return list(req);
  });

  server.on_get("/movie/{id: \\d+}", [this](const request &req) {
    return view(req);
  });

  server.on_get("/movie/edit/{id: \\d+}", [this](const request &req) {
    return edit(req);
  });

  server.on_get("/movie/delete/{id: \\d+}", [this](const request &req) {
    return remove(req);
  });

  server.on_get("/movie/create", [this](const request &req) {
    return create(req);
  });
}

matador::http::response movie_page::list(const request &)
{
  json data;
  data["title"] = "Movies";

  session s(persistence_);

  auto movies = s.select<movie>();

  json_object_mapper mapper;

  data["movies"] = mapper.to_json(movies);

  return response::ok(template_engine::render(list_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response movie_page::view(const request &p)
{
  json data;
  data["title"] = "Movie";

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto m = s.get<movie>(id);

  json_object_mapper mapper;

  data["title"] = std::string("Movie: ") + m->title;
  data["movie"] = mapper.to_json(m);

  return response::ok(template_engine::render(details_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response movie_page::create(const request &)
{
  json data;
  data["title"] = "Create Movie";

  session s(persistence_);

  auto directors = s.select<person>();

  json_object_mapper mapper;

  data["directors"] = mapper.to_json(directors);

  return response::ok(template_engine::render(create_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response movie_page::edit(const request &p)
{
  json data;
  data["title"] = "Movie";

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto m = s.get<movie>(id);

  json_object_mapper mapper;

  data["title"] = std::string("Edit Movie: ") + m->title;
  data["movie"] = mapper.to_json(m);

  auto directors = s.select<person>();

  data["directors"] = mapper.to_json(directors);

  return response::ok(template_engine::render(edit_template_, data), mime_types::TYPE_TEXT_HTML);
}

matador::http::response movie_page::remove(const request &p)
{
  json data;
  data["title"] = "Movie";

  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);

  auto m = s.get<movie>(id);

  json_object_mapper mapper;

  data["title"] = std::string("Delete Movie: ") + m->title;
  data["movie"] = mapper.to_json(m);

  return response::ok(template_engine::render(delete_template_, data), mime_types::TYPE_TEXT_HTML);
}
