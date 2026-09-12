#include "matador/query/internal/identifier_discover_service.hpp"

#include "matador/query/column_value.hpp"

namespace matador::query {
void identifier_discover_service::on_attribute(const char *id, const column_value &x, const column_options &attr) {
  if (attr.constraints().has(column_constraint::PrimaryKey)) {
    if (x.is_integer()) {
      uint64_t val;
      data_type_traits<uint64_t>::read_value(reader_, id, column_index_++, val);
      pk_ = val;
    } else if (x.is_varchar()) {
      std::string value;
      data_type_traits<std::string>::read_value(reader_, id, column_index_++, value, attr.size());
      pk_ = value;
    }
  }
  ++column_index_;
}
}
