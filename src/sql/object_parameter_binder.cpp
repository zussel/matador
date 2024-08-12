#include "matador/sql/object_parameter_binder.hpp"
#include "matador/sql/parameter_binder.hpp"

namespace matador::sql {

namespace detail {

fk_binder::fk_binder(object::attribute_binder &binder)
: binder_(binder) {}

fk_binder &fk_binder::operator=(fk_binder &&x) noexcept {
  binder_ = x.binder_;
  index_ = x.index_;

  return *this;
}

void fk_binder::on_primary_key(const char * /*id*/, std::string &value, size_t /*size*/)
{
  object::data_type_traits<std::string>::bind_value(binder_, index_++, value);
}

}

object_parameter_binder::object_parameter_binder(object::attribute_binder &binder)
: binder_(binder)
, fk_binder_(binder) {}

object_parameter_binder &object_parameter_binder::operator=(object_parameter_binder &&x) noexcept {
  index_ = x.index_;
  binder_ = x.binder_;
  fk_binder_ = std::move(x.fk_binder_);

  return *this;
}

void object_parameter_binder::reset()
{
  index_ = 0;
}

void object_parameter_binder::on_primary_key(const char * /*id*/, std::string &val, size_t size)
{
  object::data_type_traits<std::string>::bind_value(binder_, index_++, val, size);
}

void object_parameter_binder::on_revision(const char * /*id*/, unsigned long long int &rev)
{
  object::data_type_traits<unsigned long long>::bind_value(binder_, index_++, rev);
}

}