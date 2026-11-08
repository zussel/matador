#ifndef MATADOR_COLLECTION_OWNER_INFO_SETTER_HPP
#define MATADOR_COLLECTION_OWNER_INFO_SETTER_HPP

#include "matador/query/mapping/access.hpp"

#include "matador/query/model/identifier.hpp"

namespace matador::query {
class collection_owner_info_setter {
public:
  template <typename ObjectType>
  void set(ObjectType &obj, const identifier& pk) {
    pk_ = pk;
    owner_info_ = typeid(ObjectType);
    access::process(*this, obj);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename PrimaryKeyType>
  static void on_primary_key(const char * /*id*/, PrimaryKeyType &pk, const primary_key_options & = DefaultPkAttributes) {}
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template<typename T>
  static void on_attribute(const char * /*id*/, T &, const column_options & = NullAttributes) {}
  template<class P>
  static void on_belongs_to(const char * /*id*/, P &, const foreign_key_options & ) {}
  template<class P>
  static void on_has_one(const char * /*id*/, P &, const char * /*join_column*/, const foreign_key_options & ) {}
  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &objects, const char * /*join_column*/, const foreign_key_options & ) {
    objects.owner_info({owner_info_, pk_});
  }
  template<class CollectionType>
  void on_has_many_to_many(const char * /*id*/, CollectionType &objects, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options & ) {
    objects.owner_info({owner_info_, pk_});
  }
  template<class CollectionType>
  void on_has_many_to_many(const char * /*id*/, CollectionType &objects, const foreign_key_options & ) {
    objects.owner_info({owner_info_, pk_});
  }

private:
  identifier pk_{};
  std::type_index owner_info_{typeid(void)};

};
}
#endif // MATADOR_COLLECTION_OWNER_INFO_SETTER_HPP
