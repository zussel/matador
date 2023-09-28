#include "services/director_service.hpp"
#include "services/movie_service.hpp"
#include "pages/directors_page.hpp"
#include "pages/movie_page.hpp"
#include "pages/main_page.hpp"

#include "models/person.hpp"
#include "models/movie.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/static_file_service.hpp"

#include "matador/utils/os.hpp"

using namespace matador;

int main(int /*argc*/, char* /*argv*/[])
{
  try {
    // setup application logging
    matador::add_log_sink(matador::create_file_sink("log/server.log"));
    matador::add_log_sink(matador::create_stdout_sink());

    // setup database
    matador::persistence p("sqlite://moviedb.sqlite");
    p.enable_log();

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

    main_page mainpage(server, p);
    movie_page moviepage(server, p);
    directors_page directorpage(server, p);
    movie_service mservice(server, p);
    director_service dservice(server, p);

    // start server
    server.run();

    // cleanup network stack
    net::cleanup();
  } catch (const std::exception &ex) {
    std::cout << ex.what() << " (pwd: " << matador::os::get_current_dir() << ")\n";
  }
  return 0;
}
