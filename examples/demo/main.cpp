#include "services/auth_service.hpp"
#include "pages/main_page.hpp"

#include "models/person.hpp"
#include "models/movie.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/object/json_object_serializer.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/static_file_service.hpp"

using namespace matador;

int main(int /*argc*/, char* /*argv*/[])
{
  // setup application logging
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  // setup database
  matador::persistence p("sqlite://moviedb.sqlite");
  p.attach<person>("person");
  p.attach<movie>("movie");

  p.create();

  // load entities
  session s(p);
  s.load();

  // initialize network stack
  net::init();

  // create server
  http::server server(19082, "web");
  server.add_routing_middleware();

  // add routes
  http::serve_static_files_at("/css/*.*", server);
  http::serve_static_files_at("/js/*.*", server);
  http::serve_static_files_at("/fonts/*.*", server);

  server.on_get("/init", [&p](const http::request &) {
    session s(p);


    object_ptr<person> steven;
    transaction tr = s.begin();
    try {
      steven = s.insert(new person("Steven Spielberg", date(18, 12, 1946)));
      s.insert(new person("George Lucas", date(14, 5, 1944)));
      tr.commit();
    } catch (std::exception &ex) {
      log(log_level::LVL_ERROR, "Init", "Couldn't commit transaction: %s", ex.what());
      tr.rollback();
    }

    tr = s.begin();
    try {
      s.insert(new movie("Jaws", 1974, steven));
      s.insert(new movie("Raiders of the lost Arc", 1984, steven));
      tr.commit();
    } catch (std::exception &ex) {
      log(log_level::LVL_ERROR, "Init", "Couldn't commit transaction: %s", ex.what());
      tr.rollback();
    }

    return http::response::ok("app initialized", http::mime_types::TYPE_TEXT_PLAIN);
  });

  server.on_get("/list", [&p](const http::request &) {
    session s(p);

    auto result = s.select<movie>();

    for (const auto &m : result) {
      log(log_level::LVL_INFO, "List", "found movie %s (id: %d)", m->title.c_str(), m->id.value());
    }
    json_object_serializer js(json_format::pretty);

    std::string body = js.to_json(result);

    return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
  });

  main_page mpage(server, p);

  // start server
  server.run();

  // cleanup network stack
  net::cleanup();

  return 0;
}
