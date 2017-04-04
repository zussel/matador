#include "matador/object/relation_field_serializer.hpp"
//#include "matador/object/object_store.hpp"
#include "matador/object/generic_access.hpp"
#include "matador/object/object_ptr.hpp"

namespace matador {
namespace detail {

template<class T>
void relation_field_serializer::set(const std::string &field, const object_ptr<T> &obj, object_proxy *value)
{
  execute(SET, field, obj, value);
}

template<class T>
void relation_field_serializer::clear(const std::string &field, const object_ptr<T> &obj)
{
  execute(CLEAR, field, obj, nullptr);
}

template<class T>
void relation_field_serializer::append(const std::string &field, const object_ptr<T> &obj, object_proxy *value)
{
  execute(APPEND, field, obj, value);
}

template<class T>
void relation_field_serializer::remove(const std::string &field, const object_ptr<T> &obj, object_proxy *value)
{
  execute(REMOVE, field, obj, value);
}

template<class T>
void
relation_field_serializer::execute(relation_field_serializer::serialize_action action,
                                   const std::string &field, const object_ptr<T> &obj, object_proxy *value)
{
  action_ = action;
  value_ = value;
  field_ = field;

  matador::access::serialize(*this, *obj);

  field_ = "";
  value_ = nullptr;
}

}
}