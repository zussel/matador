#include "director_service.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/object/json_object_mapper.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

#include "../models/person.hpp"

using namespace matador;
using namespace matador::http;

director_service::director_service(matador::http::server &s, matador::persistence &p)
: log_(matador::create_logger("DirectorService"))
, persistence_(p)
{
  s.on_get("/api/director", [this](const request &req) {
    return list(req);
  });
  s.on_get("/api/director/{id: \\d+}", [this](const request &req) {
    return get_director(req);
  });
  s.on_post("/api/director", [this](const request &req) {
    return create_director(req);
  });
  s.on_post("/api/director/{id: \\d+}", [this](const request &req) {
    return update_director(req);
  });
  s.on_post("/api/director/delete/{id: \\d+}", [this](const request &req) {
    return delete_director(req);
  });
}

matador::http::response director_service::list(const matador::http::request &p)
{
  session s(persistence_);

  auto result = s.select<person>();

  for (const auto &m : result) {
    log(log_level::LVL_INFO, "List", "found director %s (id: %d)", m->name.c_str(), m->id);
  }
  json_object_mapper mapper;

  std::string body = mapper.to_string(result, json_format::pretty);

  return response::ok(body, mime_types::TYPE_APPLICATION_JSON);
}

matador::http::response director_service::get_director(const matador::http::request &p)
{
  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);
  auto result = s.get<person>(id);

  if (result.empty()) {
    return response::not_found();
  }
  json_object_mapper mapper;

  std::string body = mapper.to_string(result, json_format::pretty);

  return response::ok(body, mime_types::TYPE_APPLICATION_JSON);
}

matador::http::response director_service::create_director(const matador::http::request &p)
{
  auto m = new person;
  m->name = p.form_data().at("name");

  session s(persistence_);
  auto tr = s.begin();
  try {
    s.insert(m);
    tr.commit();
  } catch (std::exception &ex) {
    log_.error("Error while creating director '%s': %s", m->name.c_str(), ex.what());
    tr.rollback();
  }
  return matador::http::response::redirect("/director");
}

matador::http::response director_service::update_director(const matador::http::request &p)
{
  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);
  auto result = s.get<person>(id);

  if (result.empty()) {
    return response::not_found();
  }

  auto tr = s.begin();
  try {

    result.modify()->name = p.form_data().at("name");

    tr.commit();
  } catch (std::exception &ex) {
    log_.error("couldn't modify director %s (error %s)", result->name.c_str(), ex.what());
    tr.rollback();
  }

  return matador::http::response::redirect("/director/" + p.path_params().at("id"));
}

matador::http::response director_service::delete_director(const matador::http::request &p)
{
  auto id = std::stoul(p.path_params().at("id"));

  session s(persistence_);
  auto result = s.get<person>(id);

  if (result.empty()) {
    return response::not_found();
  }

  auto tr = s.begin();
  try {

    s.remove(result);

    tr.commit();
  } catch (std::exception &ex) {
    log_.error("Error while deleting director '%s' (id: %d): %s", result->name.c_str(), result.id(), ex.what());
    tr.rollback();
  }

  return matador::http::response::redirect("/director");
}
