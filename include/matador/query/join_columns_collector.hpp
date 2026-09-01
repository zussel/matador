#ifndef JOIN_COLUMNS_COLLECTOR_HPP
#define JOIN_COLUMNS_COLLECTOR_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/primary_key_options.hpp"

#include <string>

namespace matador::query {
struct join_columns {
  std::string join_column;
  std::string inverse_join_column;
};

class join_columns_collector final {
public:
    template<class Type>
    join_columns collect() {
        join_columns_ = {};
        Type obj;

        field::process(*this, obj);

        return join_columns_;
    }
    template<typename BaseType>
    static void on_base(const BaseType&) {}
    template < class V >
    static void on_primary_key(const char * /*id*/, V &, const primary_key_options& /*attr*/) {}
    static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
    template<typename Type>
    static void on_attribute(const char * /*id*/, Type &, const column_options &/*attr*/) {}
    template<class Pointer>
    static void on_belongs_to(const char * /*id*/, Pointer &/*obj*/, const foreign_options &/*attr*/) {}
    template<class Pointer>
    static void on_has_one(const char * /*id*/, Pointer &/*obj*/, const char * /*join_column*/, const foreign_options &/*attr*/) {}
    template<class ContainerType>
    static void on_has_many(ContainerType &, const char */*join_column*/, const foreign_options &/*attr*/) {}
    template<class ContainerType>
    void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const char *join_column, const char *inverse_join_column, const foreign_options &/*attr*/) {
        join_columns_.join_column = join_column;
        join_columns_.inverse_join_column = inverse_join_column;
    }
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const foreign_options &/*attr*/) {}

private:
    join_columns join_columns_;
};
}

#endif //JOIN_COLUMNS_COLLECTOR_HPP
