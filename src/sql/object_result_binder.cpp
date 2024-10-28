#include "matador/sql/object_result_binder.hpp"

namespace matador::sql {

namespace detail {

void fk_result_binder::on_primary_key(const char * /*id*/, std::string &value, size_t size)
{
  object::data_type_traits<std::string>::read_value(*binder_, id_, index_++, value, size);
}

}

void object_result_binder::reset()
{
  index_ = 0;
}

void object_result_binder::on_primary_key(const char *id, std::string &value, size_t size)
{
  object::data_type_traits<std::string>::read_value(*binder_, id, index_++, value, size);
}

void object_result_binder::on_revision(const char *id, unsigned long long &value)
{
  object::data_type_traits<unsigned long long int>::read_value(*binder_, id, index_++, value);
}

void object_result_binder::on_attribute(const char *id, char *value, const utils::field_attributes &attr)
{
  object::data_type_traits<char*>::read_value(*binder_, id, index_++, value, attr.size());
}

void object_result_binder::on_attribute(const char *id, std::string &value, const utils::field_attributes &attr)
{
  object::data_type_traits<std::string>::read_value(*binder_, id, index_++, value, attr.size());
}

}