#ifndef MATADOR_OBJECT_GENERATOR_HPP
#define MATADOR_OBJECT_GENERATOR_HPP

#include "matador/query/access.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/table.hpp"
#include "matador/query/default_type_traits.hpp"

#include <memory>
#include <typeindex>

namespace matador::query {
class basic_schema;
enum class null_option_type : uint8_t {
  Nullable, NotNull
};

class pk_type_determinator final {
private:
    pk_type_determinator() = default;

public:
    template<class Type>
    static utils::basic_type determine() {
        pk_type_determinator determinator;
        Type t;

        access::process(determinator, t);
        return determinator.type_;
    }

    template<typename ValueType>
    void on_primary_key(const char *, ValueType &/*pk*/, const primary_key_options& attr) {
        type_ = data_type_traits<ValueType>::type(attr.size());
    }
    static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
    template < class Type >
    static void on_attribute(const char * /*id*/, Type &/*x*/, const column_options& /*attr*/) {}
    static void on_attribute(const char * /*id*/, char * /*x*/, const column_options& /*attr*/) {}
    template<class Pointer>
    static void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options& /*attr*/) {}
    template<class Pointer>
    static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options& /*attr*/) {}
    template<class ContainerType>
    static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options& /*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/, ContainerType & /*cont*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options& /*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/, ContainerType & /*cont*/, const foreign_key_options& /*attr*/) {}

private:
    utils::basic_type type_{};
};

class table_generator {
private:
    explicit table_generator(basic_schema& repo, const std::shared_ptr<table> &t);

public:
    template < class Type >
    static std::shared_ptr<table> generate(basic_schema &repo, const std::string &name) {
        return generate(std::make_unique<Type>(), repo, name);
    }

    template < class Type >
    static std::shared_ptr<table> generate(std::unique_ptr<Type>&& t,
                                            basic_schema &repo,
                                            const std::string &name,
                                            const std::string &join_column = "",
                                            const std::string &inverse_join_column = "") {
        const std::type_index ti(typeid(Type));
        auto obj = acquire_table(repo, ti, name);
        table_generator gen(repo, obj);
        access::process(gen, *t);
        if (!join_column.empty() && !inverse_join_column.empty()) {
            gen.prepare_relation_table(join_column, inverse_join_column);
        }
        return obj;
    }

    template<typename BaseType>
    static void on_base(const BaseType&) {}
    template < class Type >
    void on_primary_key(const char *, Type &x, const primary_key_options& attr);
    void on_revision(const char *id, uint64_t &rev);

    template<typename Type>
    void on_attribute(const char *id, Type &x, const column_options &attr);

    template<typename Type>
    void on_attribute(const char *id, std::optional<Type> &x, const column_options &attr);

    template<class Pointer>
    void on_belongs_to(const char *id, Pointer &x, const foreign_key_options& /*attr*/) {
      on_foreign_key(id, x);
      create_fk_constraint<typename Pointer::value_type>(id);
    }
    template<class Pointer>
    static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options& /*attr*/) {}

    template <class Pointer>
    void on_foreign_key(const char *id, Pointer &/*x*/) {
        const auto type = pk_type_determinator::determine<typename Pointer::value_type>();
        column_constraints cs = column_constraint::ForeignKey;
        cs |= column_constraint::NotNull;
        auto &ref = table_->columns_.emplace_back(column::make_plain(
            table_.get(), id, "", type, column_options{cs}, table_->columns_.size()));
    }
    template<class ContainerType>
    static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options& /*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/, ContainerType & /*cont*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options& /*attr*/) {}
    template<class ContainerType>
    static void on_has_many_to_many(const char * /*id*/, ContainerType & /*cont*/, const foreign_key_options& /*attr*/) {}

private:
    template<typename ValueType>
    column &emplace_column(const char *id, const column_options& attr) {
        auto &ref = table_->columns_.emplace_back(column::make_plain(
            table_.get(),
            std::string(id),
            "",
            data_type_traits<ValueType>::type(attr.size()),
            attr,
            table_->columns_.size()));
        return ref;
    }

    void create_pk_constraint(const std::string& name) const;
    template<typename Type>
    void create_fk_constraint(const std::string& name) const;
    void create_unique_constraint(const std::string& name) const;

    [[nodiscard]] std::vector<column>::iterator find_column_by_name(const std::string &name) const;

    void prepare_primary_key(const column &ref, identifier &&pk) const;
    void prepare_relation_table(const std::string &join_column, const std::string &inverse_join_column) const;

    template<typename Type>
    [[nodiscard]] std::shared_ptr<table> foreign_table() const;

    static std::shared_ptr<table> acquire_table(basic_schema &repo, const std::type_index &ti, const std::string& name);
    static std::shared_ptr<table> acquire_foreign_table(basic_schema &repo,
                                                        const std::type_index &ti);
private:
    basic_schema &repo_;
    std::shared_ptr<table> table_;
};

template<typename ValueType>
void table_generator::on_primary_key(const char *id, ValueType &x, const primary_key_options& attr) {
    column_constraints cs = column_constraint::PrimaryKey;
    if (attr.generator() == generator_type::Identity) {
        cs |= column_constraint::Identity;
        cs |= column_constraint::NotNull;
    }
    auto &ref = emplace_column<ValueType>(id, { attr.size(), cs });
    prepare_primary_key(ref, identifier(x));
}

template<typename Type>
void table_generator::on_attribute(const char *id, Type &/*x*/, const column_options &attr) {
    std::ignore = emplace_column<Type>(id, attr);
}

template<typename Type>
void table_generator::on_attribute(const char *id, std::optional<Type> & /*x*/, const column_options &attr) {
    std::ignore = emplace_column<Type>(id, attr);
}

template<typename Type>
void table_generator::create_fk_constraint(const std::string& name) const {
    const auto pk_attr = find_column_by_name(name);
    if (pk_attr == std::end(table_->columns_)) {
        return;
    }
    const auto obj = foreign_table<Type>();
    constraint pk_constraint = constraint::make_fk_constraint(*table_, pk_attr->index(), *obj);
    table_->constraints_.emplace_back(pk_constraint);
}

template<typename Type>
std::shared_ptr<table> table_generator::foreign_table() const {
    const auto ti = std::type_index(typeid(Type));
    const auto obj = acquire_foreign_table(repo_, ti);
    table_generator gen(repo_, obj);
    Type t;
    access::process(gen, t);
    return obj;
}

}
#endif //MATADOR_OBJECT_GENERATOR_HPP