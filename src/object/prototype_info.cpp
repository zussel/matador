#include "matador/object/prototype_info.hpp"
#include "matador/object/relation_field_endpoint.hpp"

#include <typeindex>

namespace matador {
namespace detail {

void basic_prototype_info::register_relation_endpoint(const std::type_index &tindex,
                                                      const std::shared_ptr<basic_relation_endpoint> &endpoint)
{
  endpoints.insert(std::make_pair(tindex, endpoint));
}

void basic_prototype_info::unregister_relation_endpoint(const std::type_index &tindex)
{
  endpoints.erase(tindex);
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::find_relation_endpoint(const std::type_index &tindex) const
{
  return endpoints.find(tindex);
}

basic_prototype_info::endpoint_iterator basic_prototype_info::find_relation_endpoint(const std::type_index &tindex)
{
  return endpoints.find(tindex);
}

basic_prototype_info::endpoint_iterator basic_prototype_info::endpoint_begin()
{
  return endpoints.begin();
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::endpoint_begin() const
{
  return endpoints.begin();
}

basic_prototype_info::endpoint_iterator basic_prototype_info::endpoint_end()
{
  return endpoints.end();
}

basic_prototype_info::const_endpoint_iterator basic_prototype_info::endpoint_end() const
{
  return endpoints.end();
}
}
}