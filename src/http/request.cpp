#include "matador/http/request.hpp"

namespace matador {
namespace http {

void request::execute(response &) {

  // encode the url (replace %## with its value)

  // check path

  //
}

void request::add_path_param(const std::string &name, const std::string &param)
{
  path_param_map_.insert(std::make_pair(name, param));
}

}
}