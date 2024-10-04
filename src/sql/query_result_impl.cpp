#include "matador/sql/query_result_impl.hpp"
#include "matador/sql/query_result_reader.hpp"
#include "matador/sql/value.hpp"

namespace matador::sql {

detail::pk_reader::pk_reader(query_result_reader &reader)
: reader_(reader) {}

void detail::pk_reader::on_primary_key(const char *id, std::string &value, size_t size)
{
  object::data_type_traits<std::string>::read_value(reader_, id, column_index_++, value, size);
}

query_result_impl::query_result_impl(std::unique_ptr<query_result_reader> &&reader, std::vector<column_definition> &&prototype, const size_t column_index)
: column_index_(column_index)
, prototype_(std::move(prototype))
, reader_(std::move(reader))
, pk_reader_(*reader_)
{}

query_result_impl::query_result_impl(std::unique_ptr<query_result_reader> &&reader, const std::vector<column_definition> &prototype, const size_t column_index)
: column_index_(column_index)
, prototype_(prototype)
, reader_(std::move(reader))
, pk_reader_(*reader_)
{}

void query_result_impl::on_primary_key(const char *id, std::string &value, size_t size)
{
  object::data_type_traits<std::string>::read_value(*reader_, id, column_index_++, value, size);
}

void query_result_impl::on_revision(const char *id, unsigned long long int &rev)
{
  object::data_type_traits<unsigned long long int>::read_value(*reader_, id, column_index_++, rev);
  reader_->read_value(id, column_index_++, rev);
}

void query_result_impl::on_attribute(const char *id, char *value, const utils::field_attributes &attr)
{
  object::data_type_traits<char*>::read_value(*reader_, id, column_index_++, value, attr.size());
}

void query_result_impl::on_attribute(const char *id, std::string &value, const utils::field_attributes &attr)
{
  object::data_type_traits<std::string>::read_value(*reader_, id, column_index_++, value, attr.size());
}

void
query_result_impl::on_attribute(const char *id, value &val, const utils::field_attributes &attr)
{
  reader_->read_value(id, column_index_++, val, attr.size());
}

const std::vector<column_definition>& query_result_impl::prototype() const
{
  return prototype_;
}

}