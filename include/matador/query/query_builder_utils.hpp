#ifndef MATADOR_QUERY_BUILDER_UTILS_HPP
#define MATADOR_QUERY_BUILDER_UTILS_HPP

#include <functional>
#include <map>
#include <typeindex>

namespace matador::query {
template<class EntityType>
static std::pair<std::type_index, const void *> make_entity_visit_key(const EntityType &ptr) {
  return {std::type_index(typeid(EntityType)), static_cast<const void *>(&ptr)};
}

struct entity_visit_key_hash {
  size_t operator()(const std::pair<std::type_index, const void *> &p) const noexcept {
    const size_t h1 = p.first.hash_code();
    const size_t h2 = std::hash<const void *>{}(p.second);
    return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
  }
};

struct processing_many_to_many_key {
  std::string id;
  std::type_index local_type{typeid(void)};
  std::type_index foreign_type{typeid(void)};
  bool operator==(processing_many_to_many_key const &other) const {
    return local_type == other.local_type && foreign_type == other.foreign_type && id == other.id;
  }
};

template<class LocalType, typename ForeignType>
static processing_many_to_many_key make_processing_many_to_many_key(const std::string &id) {
  return {id, std::type_index(typeid(LocalType)), std::type_index(typeid(ForeignType))};
}

struct processing_many_to_many_key_hash {
  size_t operator()(const processing_many_to_many_key &p) const noexcept {
    size_t seed = std::hash<std::type_index>{}(p.local_type);

    const size_t foreign_hash = std::hash<std::type_index>{}(p.foreign_type);
    seed ^= foreign_hash + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

    const size_t id_hash = std::hash<std::string>{}(p.id);
    seed ^= id_hash + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

    return seed;
  }
};

struct query_builder_context {
  const basic_schema &schema_;
  const std::unordered_map<std::type_index, query_contexts> &contexts_by_type_;

  std::vector<std::unique_ptr<execute_step>> steps_{};
  std::vector<std::unique_ptr<execute_step>> relation_steps_{};
  std::unordered_set<std::pair<std::type_index, const void *>, entity_visit_key_hash> visited_{};
  std::unordered_set<processing_many_to_many_key, processing_many_to_many_key_hash> processing_many_to_many_relations_{};
};
}
#endif //MATADOR_QUERY_BUILDER_UTILS_HPP
