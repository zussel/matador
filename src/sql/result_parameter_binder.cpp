#include "matador/sql/result_parameter_binder.hpp"

namespace matador::sql {

namespace detail {
fk_result_binder::fk_result_binder(result_parameter_binder &result_binder)
: result_binder_(result_binder)
{}

void fk_result_binder::on_primary_key(const char * /*id*/, std::string &value, size_t size)
{
//  data_type_traits<std::string>::bind_result_value(result_binder_, column_index_++, value, size);
}

}

void result_parameter_binder::on_primary_key(const char * /*id*/, std::string &value, size_t size)
{
//  data_type_traits<std::string>::bind_result_value(*this, column_index_++, value, size);
}

void result_parameter_binder::on_revision(const char * /*id*/, unsigned long long &rev)
{
//  data_type_traits<unsigned long long>::bind_result_value(*this, column_index_++, rev);
}

void result_parameter_binder::on_attribute(const char * /*id*/, char *value, const utils::field_attributes &attr)
{
//  data_type_traits<char*>::bind_result_value(*this, column_index_++, value, attr.size());
}

void result_parameter_binder::on_attribute(const char * /*id*/, std::string &value, const utils::field_attributes &attr)
{
//  data_type_traits<std::string>::bind_result_value(*this, column_index_++, value, attr.size());
}

void result_parameter_binder::on_attribute(const char * /*id*/, utils::any_type &/*value*/, data_type /*type*/, const utils::field_attributes &/*attr*/)
{

}

}