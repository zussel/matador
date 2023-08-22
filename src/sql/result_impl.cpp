#include "matador/sql/result_impl.hpp"
#include "matador/sql/row.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

result_row_serializer::result_row_serializer(result_impl &impl)
: impl_(impl) { }

void result_row_serializer::serialize(row &r)
{
  r.serialize(*this);
}

void result_row_serializer::on_attribute(const char *id, char &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, short &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, int &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, long &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, long long int &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, unsigned char &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, unsigned short &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, unsigned int &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, unsigned long &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, unsigned long long int &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, bool &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, float &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, double &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, char *x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, std::string &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, time &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_attribute(const char *id, date &x, const field_attributes &attr) {
  impl_.on_attribute(id, x, attr);
}

void result_row_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type) {
  impl_.read_foreign_object(id, x);
}

void result_row_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type) {
  impl_.read_foreign_object(id, x);
}

result_impl::result_impl()
: result_identifier_reader_(*this)
, result_row_serializer_(*this)
{}

void result_impl::serialize(row &r)
{
  matador::access::serialize(result_row_serializer_, r);
}

void result_impl::on_attribute(const char *id, char *value, const field_attributes &attr)
{
  read_value(id, column_index_++, value, attr.size());
}

void result_impl::on_attribute(const char *id, std::string &value, const field_attributes &attr)
{
  if (attr.size() == 0) {
    read_value(id, column_index_++, value);
  } else {
    read_value(id, column_index_++, value, attr.size());
  }
}

void result_impl::on_belongs_to(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

void result_impl::on_has_one(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

void result_impl::read_foreign_object(const char */*id*/, identifiable_holder &x)
{
  //determine and create primary key of object ptr
  auto pk = x.primary_key();
  if (pk.is_null()) {
    pk = x.create_identifier();
  }
  pk.serialize(result_identifier_reader_);
  if (!pk.is_valid()) {
    // no pk is set => null
    pk.clear();
    x.reset(pk);
    return;
  }

  // set found primary key into object_base_ptr
  if (!x.has_primary_key()) {
    x.reset(pk);
  }
}

result_impl::size_type result_impl::column_index() const
{
  return column_index_;
}

void result_identifier_reader::serialize(null_type_t &, const field_attributes &/*attr*/) { }

}
}