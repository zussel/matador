#include "matador/query/foreign_options.hpp"

namespace matador::query {
foreign_options::foreign_options(const cascade_type cascade)
: cascade_(cascade) {}

foreign_options::foreign_options(const fetch_type fetch)
: fetch_(fetch) {}

foreign_options::foreign_options(const cascade_type cascade, const fetch_type fetch)
: cascade_(cascade)
, fetch_(fetch ) {}

cascade_type foreign_options::cascade() const {
    return cascade_;
}

fetch_type foreign_options::fetch() const {
    return fetch_;
}

}
