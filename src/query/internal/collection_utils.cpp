#include "matador/query/internal/collection_utils.hpp"

namespace matador::query {
bool collection_composite_key::operator==(const collection_composite_key &other) const {
  return root_type == other.root_type &&
         type == other.type &&
         name == other.name;
}

std::size_t collection_composite_key_hash::operator()(const collection_composite_key &k) const noexcept {
  const std::size_t h1 = std::hash<std::type_index>{}(k.root_type);
  const std::size_t h2 = std::hash<std::type_index>{}(k.type);
  const std::size_t h3 = std::hash<std::string>{}(k.name);

  // Klassische Hash-Kombination (Boost-Style)
  std::size_t seed = h1;
  seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  return seed;
}
}
