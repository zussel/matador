#include "matador/utils/foreign_attributes.hpp"

namespace matador {

foreign_attributes::foreign_attributes(cascade_type cascade)
: cascade_(cascade) {}

foreign_attributes::foreign_attributes(fetch_type fetch)
: fetch_(fetch) {}

foreign_attributes::foreign_attributes(cascade_type cascade, fetch_type fetch)
: cascade_(cascade)
, fetch_(fetch ) {}

cascade_type foreign_attributes::cascade() const
{
  return cascade_;
}

fetch_type foreign_attributes::fetch() const
{
  return fetch_;
}

}
