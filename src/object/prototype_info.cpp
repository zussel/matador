#include "matador/object/prototype_info.hpp"
#include "matador/object/relation_field_endpoint.hpp"

#include <typeindex>

namespace matador {
namespace detail {

void basic_prototype_info::register_relation_endpoint(const std::type_index &tindex,
                                                      const std::shared_ptr<basic_relation_endpoint> &endpoint)
{
  endpoints_.insert(std::make_pair(tindex, endpoint));
}

void basic_prototype_info::unregister_relation_endpoint(const std::type_index &tindex)
{
  endpoints_.erase(tindex);
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::find_relation_endpoint(const std::type_index &tindex) const
{
  return endpoints_.find(tindex);
}

basic_prototype_info::endpoint_iterator basic_prototype_info::find_relation_endpoint(const std::type_index &tindex)
{
  return endpoints_.find(tindex);
}

basic_prototype_info::endpoint_iterator basic_prototype_info::endpoint_begin()
{
  return endpoints_.begin();
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::endpoint_begin() const
{
  return endpoints_.begin();
}

basic_prototype_info::endpoint_iterator basic_prototype_info::endpoint_end()
{
  return endpoints_.end();
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::endpoint_end() const
{
  return endpoints_.end();
}

const basic_prototype_info::t_endpoint_map &basic_prototype_info::endpoints() const
{
  return endpoints_;
}

}
}