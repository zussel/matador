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
  typedef tree<route_path_ptr> t_route_tree;
  typedef t_route_tree::iterator iterator;

public:
  routing_engine();

  void add(const std::string& path, http::http::method_t method, const route_path::t_request_handler& request_handler);

  iterator find(const std::string &path, http::http::method_t method, route_path::t_path_param_map &path_params);

  void dump(std::ostream &out);

  bool valid(const iterator& it) const;

private:

  iterator find_internal(const std::string &path, http::http::method_t method, route_path::t_path_param_map &path_params);

  route_path_ptr make_route_path(const std::string &name, http::method_t method, const std::string &path,
                                 const route_path::t_request_handler &request_handler);

private:
  t_route_tree route_tree_;

  std::regex route_regex_;

  std::vector<route_path_ptr> routes_;
};

}
}
#endif //MATADOR_ROUTING_ENGINE_HPP
