#ifndef MATADOR_ROUTING_ENGINE_HPP
#define MATADOR_ROUTING_ENGINE_HPP

#include "matador/http/route_path.hpp"

#include "matador/utils/tree.hpp"

#include <string>
#include <list>

namespace matador {
namespace http {

class routing_engine
{
public:
  typedef std::shared_ptr<route_path> route_path_ptr;

  routing_engine();

  void add(const std::string& path, http::http::method_t method, const route_path::t_request_handler& request_handler);

  route_path_ptr find(const std::string &path, http::http::method_t method, request &req);

  bool contains(const std::string &path, http::http::method_t method);

  void dump(std::ostream &out);

private:
  typedef tree<route_path_ptr> t_route_tree;

  t_route_tree::iterator find_internal(const std::string &path, http::http::method_t method, request &req);

  route_path_ptr make_route_path(const std::string &name, http::method_t method, const std::string &path,
                                 const route_path::t_request_handler &request_handler);

private:
  t_route_tree route_tree_;

  std::regex route_regex_;
};

bool is_path_param(const std::string &elem);

}
}
#endif //MATADOR_ROUTING_ENGINE_HPP
