#include <chrono>

#include "matador/logger/log_manager.hpp"

#include "matador/utils/json_mapper.hpp"
#include "matador/utils/url.hpp"

#include "matador/object/has_many.hpp"
#include "matador/object/json_object_mapper.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/http/static_file_service.hpp"
#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/middleware.hpp"

/*
 * curl -v -H "Content-Type: application/json" --data '{"username":"otto","password":"otto123"}' -X POST http://localhost:7091/api/v1/auth/login?query=9
 */

using namespace matador;
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

struct person
  {
  identifier<long> id;   // primary key
  std::string name;
  date birthday;
  has_many<std::string> colors {};

  person() = default;
  person(long i, std::string n)
  : id(i), name(std::move(n))
  {}

  template < class SERIALIZER >
    void serialize(SERIALIZER &serializer) {
    serializer.serialize("id", id);
    serializer.serialize("name", name, 255);
    serializer.serialize("birthday", birthday);
    serializer.serialize("person_color",  // relation table name
                         colors,          // class member
                         "person_id",     // left column in relation table
                         "color",         // right column in relation table
                         matador::cascade_type::ALL); // cascade type
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
  explicit auth_service(http::server &s)
    : server_(s)
    , log_(matador::create_logger("AuthService"))
  {
    s.on_post("/api/v1/auth/login", [this](const http::request &req) { return login(req); });
    s.on_post("/api/v1/auth/logout", [this](const http::request &req) { return logout(req); });
  }

  http::response login(const http::request &req)
  {
    log_.info("login");

    matador::json_mapper mapper;

    auto credentials = mapper.to_object<credential>(req.body().c_str());

    log_.info("user %s logging in", credentials.username.c_str());

    // extract username and password from headers
//    req.headers.at("Authentication");

    user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};

    return http::response::json(u);
  }

  http::response logout(const http::request &)
  {
    log_.info("logout");
    return http::response::no_content();
  }

private:
  http::server& server_;

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
  http::server server_;
  auth_service auth_;
  http::static_file_service app_;
};

class log_middleware : public http::middleware
{
public:
  log_middleware()
    : log_(matador::create_logger("LogMiddleware"))
  {}

  matador::http::response process(http::request &, const next_func_t &next) override
  {
    log_.info("before LogMiddleware");
    auto resp = next();
    log_.info("after LogMiddleware");
    return resp;
  }

private:
  matador::logger log_;
};

class time_measure_middleware : public http::middleware
{
public:
  time_measure_middleware()
    : log_(matador::create_logger("TimeMeasureMiddleware"))
  {}

  matador::http::response process(http::request &, const next_func_t &next) override
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

//  e_numbers en = ONE;
//
//  s(en);
//
//  matador::add_log_sink(matador::create_stdout_sink());
//
//  http::middleware_pipeline pipeline;
//
//  pipeline.add(std::make_shared<log_middleware>());
//  pipeline.add(std::make_shared<time_measure_middleware>());
//
//  matador::http::request req;
//
//  auto resp = pipeline.process(req);

  // creates a web application at port 7091
//  application app(7091);

//  app.run();

  add_log_sink(matador::create_stdout_sink());

  // prepare the persistence layer
  persistence p("sqlite://db.sqlite");
  p.enable_log();
  p.attach<person>("person");

  // create tables
  p.create();

  // create a database session and
  // load data from db
  session s(p);
  s.load();

  // initialize network stack
  net::init();

  // create server and add routing
  http::server server(8700);
  server.add_routing_middleware();

  // return all persons
  server.on_get("/person", [&s](const http::request &) {
    auto result = s.select<person>();
    json_object_mapper mapper;

    std::string body = mapper.to_string(result, json_format::pretty);

    return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
  });

  // return one person
  server.on_get("/person/{id: \\d+}", [&s](const http::request &req) {
    auto id = std::stoul(req.path_params().at("id"));
    auto result = s.get<person>(id);

    if (result.empty()) {
      return http::response::not_found();
    }

    json_object_mapper mapper;

    std::string body = mapper.to_string(result, json_format::pretty);

    return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
  });

  // insert person
  server.on_post("/person", [&s](const http::request &req) {
    json_object_mapper mapper;

    auto p = mapper.to_ptr<person>(req.body());

    std::string body;
    auto tr = s.begin();
    try {
      auto optr = s.insert(p.release());
      tr.commit();
      body = mapper.to_string(optr, json_format::pretty);
    } catch (std::exception &ex) {
      tr.rollback();
    }
    return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
  });

  // update person
  server.on_put("/person/{id: \\d+}", [&s](const http::request &req) {
    auto id = std::stoul(req.path_params().at("id"));
    auto result = s.get<person>(id);

    if (result.empty()) {
      return http::response::not_found();
    }

    json_object_mapper mapper;
    auto p = mapper.to_object<person>(req.body());

    // update entity
    // ...

    return http::response::no_content();
  });

  // delete person
  server.on_remove("/person/{id: \\d+}", [&s](const http::request &req) {
    auto id = std::stoul(req.path_params().at("id"));
    auto result = s.get<person>(id);

    if (result.empty()) {
      return http::response::not_found();
    }

    s.remove(result);

    return http::response::no_content();
  });

  server.run();

  net::cleanup();

  return 0;
}
