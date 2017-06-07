#include "matador/object/relation_field_endpoint.hpp"


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
}
}