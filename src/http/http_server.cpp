#include "matador/http/http_server.hpp"
#include "matador/http/http_connection.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

server::server(unsigned short port)
  : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  method_route_map_.insert(std::make_pair(http::GET, tree<route_endpoint>()));
  method_route_map_.insert(std::make_pair(http::POST, tree<route_endpoint>()));
  method_route_map_.insert(std::make_pair(http::PUT, tree<route_endpoint>()));
  method_route_map_.insert(std::make_pair(http::DELETE, tree<route_endpoint>()));

  service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<http_connection>(stream, std::move(ep));
    conn->start();
  });
}

void server::run()
{
  service_.run();
}

void server::get(const std::string &route, server::t_request_handler handler)
{
//  method_route_map_.at(http::GET).
}

void server::post(const std::string &route, server::t_request_handler handler)
{

}

void server::put(const std::string &route, server::t_request_handler handler)
{

}

void server::remove(const std::string &route, server::t_request_handler handler)
{

}
}
}
