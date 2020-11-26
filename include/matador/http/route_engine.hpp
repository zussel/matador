#ifndef MATADOR_ROUTE_ENGINE_HPP
#define MATADOR_ROUTE_ENGINE_HPP

#include "matador/http/route_endpoint.hpp"

#include "matador/utils/tree.hpp"

#include <string>
#include <list>

namespace matador {
namespace http {

class route_engine
{
public:
  route_engine() = default;

  void add(const std::string& path, http::http::method_t method, route_endpoint::t_request_handler request_handler);

  route_endpoint find(const std::string &path, http::http::method_t method);

  bool contains(const std::string &path, http::http::method_t method);

  void dump(std::ostream &out);

private:
  typedef tree<route_endpoint> t_route_tree;

  t_route_tree::iterator find_internal(const std::string &path, http::http::method_t method);

private:
  t_route_tree route_tree_;
};

bool is_path_param(const std::string &elem);

}
}
#endif //MATADOR_ROUTE_ENGINE_HPP
