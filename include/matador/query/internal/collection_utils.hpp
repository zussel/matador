#ifndef MATADOR_COLLECTION_UTILS_HPP
#define MATADOR_COLLECTION_UTILS_HPP

#include <string>
#include <typeindex>

namespace matador::query {
struct collection_composite_key {
  std::type_index root_type;
  std::type_index type;
  std::string name;

  bool operator==(const collection_composite_key& other) const;
};

struct collection_composite_key_hash {
  std::size_t operator()(const collection_composite_key& k) const noexcept;
};
}
#endif //MATADOR_COLLECTION_UTILS_HPP