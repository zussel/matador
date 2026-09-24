#ifndef MATADOR_OBJECT_PK_BINDER_HPP
#define MATADOR_OBJECT_PK_BINDER_HPP

#include "matador/query/access.hpp"
#include "matador/query/value_writer.hpp"
#include "matador/query/default_type_traits.hpp"
#include "matador/query/primary_key_options.hpp"

namespace matador::query {
class column_options;
class foreign_key_options;

class primary_key_binder {
public:
    template<class Type>
    void bind(Type &obj, const size_t column_index, value_writer &binder) {
        binder_ = &binder;
        index_ = column_index;
        access::process(*this, obj);
        binder_ = nullptr;
    }

    template<typename BaseType>
    static void on_base(const BaseType&) {}
    template<typename ValueType>
    void on_primary_key(const char *id, ValueType &value, const primary_key_options& attr);
    static void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}

    template < class Type >
    static void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {}
    template < class Pointer >
    static void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options &/*attr*/) {}
    template < class Pointer >
    static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}

    template<class ContainerType>
    static void on_has_many(const char * /*id*/,
                            ContainerType &/*c*/,
                            const char * /*join_column*/,
                            const foreign_key_options &/*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/,
                                    ContainerType &/*c*/,
                                    const char * /*join_column*/,
                                    const char * /*inverse_join_column*/,
                                    const foreign_key_options &/*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/,
                                    ContainerType &/*c*/,
                                    const foreign_key_options &/*attr*/) {}

private:
    value_writer *binder_{};
    size_t index_{0};
};

template<typename ValueType>
void primary_key_binder::on_primary_key(const char * /*id*/, ValueType &value, const primary_key_options& attr) {
    data_type_traits<ValueType>::bind_value(*binder_, index_++, value, attr.size());
}

}
#endif //MATADOR_OBJECT_PK_BINDER_HPP