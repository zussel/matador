#include <utility>
#include <chrono>
#include <locale>

#include "matador/logger/log_manager.hpp"

#include "matador/utils/json_mapper.hpp"

#include "matador/http/static_file_service.hpp"
#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/url.hpp"
#include "matador/http/middleware.hpp"

/*
 * curl -v -H "Content-Type: application/json" --data '{"username":"otto","password":"otto123"}' -X POST http://localhost:7091/api/v1/auth/login?query=9
 */

using namespace matador::http;
using namespace std;

struct user
{
  long id;
  std::string username;
  std::string password;
  std::string first_name;
  std::string last_name;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("username", username);
    serializer.serialize("password", password);
    serializer.serialize("first_name", first_name);
    serializer.serialize("last_name", last_name);
  }
};

struct credential
{
  std::string username;
  std::string password;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("username", username);
    serializer.serialize("password", password);
  }
};

class auth_service
{
public:
  explicit auth_service(server &s)
    : server_(s)
    , log_(matador::create_logger("AuthService"))
  {
    s.on_post("/api/v1/auth/login", [this](const request &req) { return login(req); });
    s.on_post("/api/v1/auth/logout", [this](const request &req) { return logout(req); });
  }

  response login(const request &req)
  {
    log_.info("login");

    matador::json_mapper mapper;

    auto credentials = mapper.to_object<credential>(req.body().c_str());

    log_.info("user %s logging in", credentials.username.c_str());

    // extract username and password from headers
//    req.headers.at("Authentication");

    user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};

    return response::json(u);
  }

  response logout(const request &)
  {
    log_.info("logout");
    return response::no_content();
  }

private:
  server& server_;

  matador::logger log_;
};

class application
{
public:
  explicit application(unsigned short port)
    : server_(port)
    , auth_(server_)
    , app_("/app/*.*", server_)
  {}

  void run()
  {
    server_.run();
  }

private:
  server server_;
  auth_service auth_;
  static_file_service app_;
};

class log_middleware : public middleware
{
public:
  log_middleware()
    : log_(matador::create_logger("LogMiddleware"))
  {}

  matador::http::response process(request &, const next_func_t &next) override
  {
    log_.info("before LogMiddleware");
    auto resp = next();
    log_.info("after LogMiddleware");
    return resp;
  }

private:
  matador::logger log_;
};

class time_measure_middleware : public middleware
{
public:
  time_measure_middleware()
    : log_(matador::create_logger("TimeMeasureMiddleware"))
  {}

  matador::http::response process(request &, const next_func_t &next) override
  {
    auto start = std::chrono::high_resolution_clock::now();

    log_.info("before LogMiddleware");
    auto resp = next();
    log_.info("after LogMiddleware");
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    log_.info("processing middleware took (%dµs)", elapsed);
    return resp;
  }

private:
  matador::logger log_;
};

enum e_numbers { ONE, TWO, THREE };

void s(int ) {

}

int main(int /*argc*/, char* /*argv*/[])
{
//  std::string umlaut{ "aeiäöü" };
//
//  unsigned char comp = 128;
//  for (unsigned char c : umlaut) {
//      bool is_ascii = (c & comp) == 0;
//      auto b = isalnum((int)c);
//      std::cout << "'" << c << "' isalnum: " << b << ", is ascii: " << is_ascii << "\n";
//  }

  e_numbers en = ONE;

  s(en);

  matador::file f("timo.json", "r");

  auto text = matador::read_as_text(f);

  f.close();

  matador::json_parser p;

  auto timo = p.parse(text);

  //auto alter = timo["alter"].as<int>();

  auto freunde = timo["freunde"];

  for (const auto &ff : freunde) {
    std::cout << "freund: " << ff.as<std::string>() << "\n";
  }
  return 0;

  matador::add_log_sink(matador::create_stdout_sink());

  middleware_pipeline pipeline;

  pipeline.add(std::make_shared<log_middleware>());
  pipeline.add(std::make_shared<time_measure_middleware>());

  matador::http::request req;

  auto resp = pipeline.process(req);

  // creates a web application at port 7091
//  application app(7091);

//  app.run();
}
