#include "matador/sql/object_parameter_binder.hpp"
#include "matador/sql/parameter_binder.hpp"

namespace matador::sql {

namespace detail {

void fk_binder::on_primary_key(const char * /*id*/, std::string &value, size_t /*size*/)
{
  utils::data_type_traits<std::string>::bind_value(*binder_, index_++, value);
}

}

void object_parameter_binder::reset(size_t start_index)
{
  index_ = start_index;
}

void object_parameter_binder::on_primary_key(const char * /*id*/, std::string &val, const size_t size)
{
  utils::data_type_traits<std::string>::bind_value(*binder_, index_++, val, size);
}

void object_parameter_binder::on_revision(const char * /*id*/, unsigned long long int &rev)
{
  utils::data_type_traits<unsigned long long>::bind_value(*binder_, index_++, rev);
}

}