#include "matador/query/internal/result_binder.hpp"

namespace matador::query {
void object_result_binder::reset() {
  index_ = 0;
}

void object_result_binder::on_revision(const char *id, uint64_t &value) {
  data_type_traits<uint64_t>::read_value(*binder_, id, index_++, value);
}

void object_result_binder::on_attribute(const char *id, char *value, const column_options &attr) {
  data_type_traits<char *>::read_value(*binder_, id, index_++, value, attr.size());
}

void object_result_binder::on_attribute(const char *id, std::string &value,
                                        const column_options &attr) {
  data_type_traits<std::string>::read_value(*binder_, id, index_++, value, attr.size());
}

void object_result_binder::on_attribute(const char * /*id*/, column_value & /*val*/, const column_options & /*attr*/) {
  // utils::data_type_traits<utils::value>::read_value(*binder_, id, index_++, val);
}

} // namespace matador::sql