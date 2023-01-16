#include "matador/sql/result_impl.hpp"

#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/basic_identifier.hpp"

namespace matador {
namespace detail {

void result_impl::on_attribute(const char*, char *value, size_t s)
{
  read_value(result_index_++, result_row_, value, s);
}

void result_impl::on_attribute(const char*, std::string &value, size_t s)
{
  read_value(result_index_++, result_row_, value, s);
}

void result_impl::on_attribute(const char*, matador::time &value)
{
  read_value(result_index_++, result_row_, value);
}

void result_impl::on_attribute(const char*, matador::date &value)
{
  read_value(result_index_++, result_row_, value);
}

void result_impl::on_primary_key(const char*, matador::basic_identifier &x)
{

}

void result_impl::on_belongs_to(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

void result_impl::on_has_one(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}


void result_impl::read_foreign_object(const char *id, identifiable_holder &x)
{
  //determine and create primary key of object ptr
  basic_identifier* pk = x.primary_key();
  if (!pk) {
    pk = x.create_identifier();
  }

//  pk->serialize(id, *this);
  if (!pk->is_valid()) {
    // no pk is set => null
    return;
  }

  // set found primary key into object_base_ptr
  if (!x.has_primary_key()) {
    x.reset(pk);
  }
}

}
}