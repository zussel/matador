#ifndef MATADOR_SERIALIZATION_HPP
#define MATADOR_SERIALIZATION_HPP

#include <string>

namespace matador {

template<class T, template<class ...> class C>
class has_many;

template<class T>
class object_ptr;

enum class cascade_type;

namespace store {

template<typename T, typename Operation>
void primary_key(Operation &op, const char *name, T &value)
{
  op.on_primary_key(name, value);
}

template<typename Operation>
void revision(Operation &op, const char *name, unsigned long rev) {
  op.on_revision(name, rev);
}

template<typename Operation>
void attribute(Operation &op, const char *name, char *data, size_t len)
{
  op.on_attribute(name, data, len);
}

template<typename Operation>
void attribute(Operation &op, const char *name, std::string &data, size_t len)
{
  op.on_attribute(name, data, len);
}

template<typename T, typename Operation>
void attribute(Operation &op, const char *name, T &pk)
{
  op.on_attribute(name, pk);
}

template<typename T, typename Operation>
void has_one(Operation &op, const char *name, matador::object_ptr<T> &object, matador::cascade_type cascade)
{
  op.on_has_one(name, object, cascade);
}

template<typename T, typename Operation>
void belongs_to(Operation &op, const char *name, matador::object_ptr<T> &object, matador::cascade_type cascade)
{
  op.on_belongs_to(name, object, cascade);
}

//template< typename T, typename Operation  >
//void has_many(Operation &op, const char* name, matador::has_many<T> &object, matador::cascade_type cascade)
//{
//  op.on_has_one(name, object, cascade);
//}

template<typename T, template<class ...> class C, typename Operation>
void has_many(Operation &op, const char *name, matador::has_many<T, C> &container, const char *left_column, const char *right_column, matador::cascade_type cascade)
{
  op.on_has_many(name, container, left_column, right_column, cascade);
}

}

}

#endif //MATADOR_SERIALIZATION_HPP