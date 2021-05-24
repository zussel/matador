#include "auth_service.hpp"

#include "../models/credential.hpp"
#include "../models/user.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/utils/json_mapper.hpp"

using namespace matador;
using namespace matador::http;

auth_service::auth_service(server &s)
  : log_(matador::create_logger("AuthService"))
{
  s.on_post("/api/v1/auth/login", [this](const request &req) { return login(req); });
  s.on_post("/api/v1/auth/logout", [this](const request &req) { return logout(req); });
}

response auth_service::login(const request &req)
{
  log_.info("login");

  matador::json_mapper mapper;

//  auto username = req.form_data().at("uname");
//  auto password = req.form_data().at("pwd");

//  return http::response::redirect("secure");

  auto credentials = mapper.to_object<credential>(req.body());

  log_.info("user %s logging in", credentials.username.c_str());

  // extract username and password from headers
//    req.headers.at("Authentication");

  // doesn't work under win
  //user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};
  user u;
  u.id = 0;
  u.username = "herb";
  u.password = "herb123";
  u.first_name = "Herbert";
  u.last_name = "Grönemeyer";

  return response::json(u);
}

response auth_service::logout(const request &)
{
  log_.info("logout");
  return response::no_content();
}