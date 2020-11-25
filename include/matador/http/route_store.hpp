#ifndef MATADOR_ROUTE_STORE_HPP
#define MATADOR_ROUTE_STORE_HPP

#include "matador/http/route_endpoint.hpp"

#include "matador/utils/tree.hpp"

#include <string>

namespace matador {
namespace http {

class route_store
{
public:
  explicit route_store(std::string context_root);

  void add(const std::string& path, http::http::method_t method, route_endpoint::t_request_handler request_handler);

  route_endpoint find(const std::string &path, http::http::method_t method);

  bool contains(const std::string &path, http::http::method_t method);

  void dump(std::ostream &out);

private:
  typedef tree<route_endpoint> t_route_tree;

  t_route_tree::iterator find_interal(const std::string &path, http::http::method_t method);

private:
  std::string context_root_;
  t_route_tree route_tree_;
};

}
}
#endif //MATADOR_ROUTE_STORE_HPP
