#include "matador/query/internal/object_binder.hpp"

namespace matador::query {
void object_binder::reset(const size_t start_index) {
  index_ = start_index;
}

size_t object_binder::current_index() const {
  return index_;
}

void object_binder::on_revision(const char * /*id*/, uint64_t &rev) {
  data_type_traits<uint64_t>::bind_value(*binder_, index_++, rev);
}

}