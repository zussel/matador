#include "matador/query/internal/query_result_impl.hpp"

namespace matador::query {
query_result_impl::query_result_impl(std::unique_ptr<query_result_reader> &&reader,
                                     std::vector<column> prototype,
                                     const std::shared_ptr<resolver_service>& resolver,
                                     const std::type_index& result_type,
                                     const size_t column_index)
: column_index_(column_index)
, prototype_(std::move(prototype))
, reader_(std::move(reader))
, resolver_(resolver)
, result_type_(result_type)
, id_reader_(*reader_)
, pk_reader_(*reader_) {
}

void query_result_impl::on_revision(const char *id, uint64_t &rev) {
  data_type_traits<uint64_t>::read_value(*reader_, id, column_index_++, rev);
  reader_->read_value(id, column_index_++, rev);
}

void query_result_impl::on_attribute(const char *id, char *value, const column_options &attr) {
  data_type_traits<char *>::read_value(*reader_, id, column_index_++, value, attr.size());
}

void query_result_impl::on_attribute(const char *id, std::string &value, const column_options &attr) {
  data_type_traits<std::string>::read_value(*reader_, id, column_index_++, value, attr.size());
}

void
query_result_impl::on_attribute(const char *id, column_value &val, const column_options &attr) {
  reader_->read_value(id, column_index_++, val, attr.size());
}

bool query_result_impl::fetch(record &rec) {
  if (auto fetched = reader_->fetch(); !fetched.is_ok() || !*fetched) {
    return false;
  }

  column_index_ = reader_->start_column_index();
  access::process(*this, rec);
  return true;
}

const std::vector<column> &query_result_impl::prototype() const {
  return prototype_;
}
}