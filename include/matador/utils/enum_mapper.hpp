#ifndef QUERY_ENUM_MAPPER_HPP
#define QUERY_ENUM_MAPPER_HPP

#include <optional>
#include <string>
#include <unordered_map>

namespace matador::utils {

template < typename EnumType, class Enable = void >
class enum_mapper;

template < typename EnumType>
class enum_mapper<EnumType, std::enable_if_t<std::is_enum_v<EnumType>>>
{
public:
  using enum_to_string_map = std::unordered_map<EnumType, std::string>;
  using string_to_enum_map = std::unordered_map<std::string, EnumType>;

  explicit enum_mapper(enum_to_string_map enum_map)
  : enum_to_string_map_(std::move(enum_map))
  {
    for (const auto &elem : enum_to_string_map_) {
      string_to_enum_map_.emplace(elem.second, elem.first);
    }
  }

  std::optional<EnumType> to_enum(const std::string &enum_string) const {
    if (const auto it = string_to_enum_map_.find(enum_string); it != string_to_enum_map_.end()) {
      return { it->second };
    }

    return std::nullopt;
  }

  const std::string& to_string(EnumType enum_value) const {
    if (const auto it = enum_to_string_map_.find(enum_value); it != enum_to_string_map_.end()) {
      return it->second;
    }

    return empty_;
  }

private:
  enum_to_string_map enum_to_string_map_;
  string_to_enum_map string_to_enum_map_;
  std::string empty_{};
};

}
#endif //QUERY_ENUM_MAPPER_HPP
