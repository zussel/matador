#ifndef MATADOR_BINARY_DESERIALIZER_HPP
#define MATADOR_BINARY_DESERIALIZER_HPP

#include "matador/query/mapping/access.hpp"
#include "matador/query/mapping/binary_serializer.hpp"
#include "matador/query/model/identifier.hpp"
#include "matador/query/relations/collection.hpp"
#include "matador/query/relations/object_ptr.hpp"
#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/types.hpp"

#include <charconv>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

namespace matador::query {
/**
 * Restores objects from a binary_serializer snapshot. The input map is copied,
 * while its buffer_views retain ownership of the serialized bytes. string_view
 * model fields therefore refer into this deserializer's snapshot and must not
 * outlive it. Attribute bytes use native representations and are not portable
 * between different platforms or compiler ABIs.
 */
class binary_deserializer {
public:
  using key_type = binary_serializer::key_type;
  using map_type = binary_serializer::map_type;

  explicit binary_deserializer(const map_type &objects)
  : objects_(objects) {}

  explicit binary_deserializer(const binary_serializer &serializer)
  : binary_deserializer(serializer.objects()) {}

  template<class Type>
  object_ptr<Type> deserialize(const identifier &pk) {
    try {
      return deserialize_object<Type>(pk);
    } catch (...) {
      cache_.clear();
      throw;
    }
  }

  void clear() noexcept {
    cache_.clear();
  }

private:
  class byte_reader {
  public:
    explicit byte_reader(utils::buffer_view view)
    : view_(std::move(view)) {}

    template<class Type>
    Type read() {
      static_assert(std::is_trivially_copyable_v<Type>);
      Type value{};
      read_bytes(&value, sizeof(Type));
      return value;
    }

    bool read_bool() {
      char bytes[sizeof(bool)]{};
      read_bytes(bytes, sizeof(bool));
      if (sizeof(bool) == 1 && bytes[0] != 0 && bytes[0] != 1) {
        throw std::runtime_error("binary_deserializer: invalid boolean value");
      }
      bool value{};
      std::memcpy(&value, bytes, sizeof(bool));
      return value;
    }

    std::string_view read_string() {
      const auto length = read<std::uint64_t>();
      if (length > std::numeric_limits<std::size_t>::max() || length > remaining()) {
        throw std::runtime_error("binary_deserializer: invalid string length");
      }
      const auto size = static_cast<std::size_t>(length);
      const auto *data = view_.data() + offset_;
      offset_ += size;
      return {data, size};
    }

    void read_bytes(void *destination, std::size_t size) {
      if (size > remaining()) {
        throw std::runtime_error("binary_deserializer: unexpected end of data");
      }
      if (size != 0) {
        std::memcpy(destination, view_.data() + offset_, size);
        offset_ += size;
      }
    }

    [[nodiscard]] std::size_t remaining() const noexcept {
      return view_.size() - offset_;
    }

    [[nodiscard]] bool finished() const noexcept {
      return offset_ == view_.size();
    }

  private:
    utils::buffer_view view_;
    std::size_t offset_{};
  };

  class object_reader {
  public:
    object_reader(binary_deserializer &deserializer, const identifier &expected_key, utils::buffer_view view)
    : deserializer_(deserializer)
    , expected_key_(expected_key)
    , reader_(std::move(view)) {}

    template<class Base>
    void on_base(const Base &base) {
      access::process(*this, base);
    }

    template<class Type>
    void on_primary_key(const char *name, Type &pk, const primary_key_options &) {
      read_field(name);
      read_value(pk);
      using value_type = std::decay_t<Type>;
      if constexpr (is_identifier_supported_v<value_type>) {
        if (identifier(pk) != expected_key_) {
          throw std::runtime_error("binary_deserializer: primary key does not match map key");
        }
      } else {
        throw std::runtime_error("binary_deserializer: unsupported primary key type");
      }
    }

    void on_revision(const char *name, std::uint64_t &revision) {
      read_field(name);
      read_value(revision);
    }

    template<class Type>
    void on_attribute(const char *name, Type &value, const column_options &) {
      read_field(name);
      read_value(value);
    }

    template<class Type>
    void on_belongs_to(const char *name, Type &relation, const foreign_key_options &) {
      read_field(name);
      read_related_value(relation);
    }

    template<class Type>
    void on_has_one(const char *name, Type &relation, const char *, const foreign_key_options &) {
      read_field(name);
      read_related_value(relation);
    }

    template<class CollectionType>
    void on_has_many(const char *name, CollectionType &relations, const char *, const foreign_key_options &) {
      read_field(name);
      read_collection(relations);
    }

    template<class CollectionType>
    void on_has_many(const char *name, CollectionType &relations, const foreign_key_options &) {
      read_field(name);
      read_collection(relations);
    }

    template<class CollectionType>
    void on_has_many_to_many(const char *name, CollectionType &relations, const char *, const char *, const foreign_key_options &) {
      read_field(name);
      read_collection(relations);
    }

    template<class CollectionType>
    void on_has_many_to_many(const char *name, CollectionType &relations, const foreign_key_options &) {
      read_field(name);
      read_collection(relations);
    }

    void finish() const {
      if (!reader_.finished()) {
        throw std::runtime_error("binary_deserializer: trailing object data");
      }
    }

  private:
    void read_field(const char *name) {
      const auto field = reader_.read_string();
      const std::string_view expected = name ? std::string_view(name) : std::string_view{};
      if (field != expected) {
        throw std::runtime_error("binary_deserializer: unexpected field name or order");
      }
    }

    template<class Type>
    void read_value(std::optional<Type> &value) {
      if (reader_.read_bool()) {
        Type present{};
        read_value(present);
        value = std::move(present);
      } else {
        value.reset();
      }
    }

    void read_value(std::string &value) {
      const auto bytes = reader_.read_string();
      value.assign(bytes.data(), bytes.size());
    }

    void read_value(std::string_view &value) {
      value = reader_.read_string();
    }

    void read_value(utils::blob_type_t &value) {
      const auto length = reader_.read<std::uint64_t>();
      if (length > std::numeric_limits<std::size_t>::max() || length > reader_.remaining()) {
        throw std::runtime_error("binary_deserializer: invalid blob length");
      }
      const auto size = static_cast<std::size_t>(length);
      value.resize(size);
      if (size != 0) {
        reader_.read_bytes(value.data(), size);
      }
    }

    template<class Type>
    void read_value(Type &value) {
      if constexpr (std::is_same_v<std::decay_t<Type>, bool>) {
        value = reader_.read_bool();
      } else {
        static_assert(std::is_trivially_copyable_v<Type>, "binary_deserializer supports only trivially copyable attributes, strings, blobs, and optionals");
        value = reader_.template read<Type>();
      }
    }

    template<class Type>
    void read_related_value(Type &relation) {
      using value_type = std::decay_t<Type>;
      if constexpr (is_object_ptr<value_type>::value) {
        read_reference(relation);
      } else if constexpr (std::is_same_v<value_type, std::string> ||
                           std::is_same_v<value_type, std::string_view> ||
                           std::is_same_v<value_type, utils::blob_type_t> ||
                           std::is_trivially_copyable_v<value_type>) {
        read_value(relation);
      } else {
        const auto pk = read_identifier();
        auto object = deserializer_.template deserialize_object<value_type>(pk);
        relation = *object;
      }
    }

    template<class Type>
    void read_reference(object_ptr<Type> &relation) {
      if (!reader_.read_bool()) {
        relation = nullobj;
        return;
      }

      const auto pk = read_identifier();
      if constexpr (detail::is_complete_type<Type>::value) {
        relation = deserializer_.template deserialize_object<Type>(pk);
      } else {
        throw std::runtime_error("binary_deserializer: relation target type is incomplete");
      }
    }

    template<class CollectionType>
    void read_collection(CollectionType &relations) {
      const auto count = reader_.template read<std::uint64_t>();
      if (count > std::numeric_limits<std::size_t>::max()) {
        throw std::runtime_error("binary_deserializer: invalid collection size");
      }
      for (std::uint64_t index = 0; index < count; ++index) {
        typename CollectionType::value_type relation{};
        read_related_value(relation);
        relations.push_back(relation);
      }
    }

    template<class Type>
    identifier parse_identifier_value(std::string_view value) {
      Type parsed{};
      const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
      if (result.ec != std::errc{} || result.ptr != value.data() + value.size()) {
        throw std::runtime_error("binary_deserializer: invalid primary key value");
      }
      return identifier(parsed);
    }

    identifier read_identifier() {
      const auto type = static_cast<utils::basic_type>(reader_.template read<std::uint8_t>());
      const auto value = reader_.read_string();
      identifier pk;
      switch (type) {
        case utils::basic_type::Int8: pk = parse_identifier_value<std::int8_t>(value); break;
        case utils::basic_type::Int16: pk = parse_identifier_value<std::int16_t>(value); break;
        case utils::basic_type::Int32: pk = parse_identifier_value<std::int32_t>(value); break;
        case utils::basic_type::Int64: pk = parse_identifier_value<std::int64_t>(value); break;
        case utils::basic_type::UInt8: pk = parse_identifier_value<std::uint8_t>(value); break;
        case utils::basic_type::UInt16: pk = parse_identifier_value<std::uint16_t>(value); break;
        case utils::basic_type::UInt32: pk = parse_identifier_value<std::uint32_t>(value); break;
        case utils::basic_type::UInt64: pk = parse_identifier_value<std::uint64_t>(value); break;
        case utils::basic_type::Varchar:
        case utils::basic_type::Text:
          pk = identifier(std::string(value));
          break;
        default:
          throw std::runtime_error("binary_deserializer: unsupported primary key type");
      }
      if (!pk.is_valid()) {
        throw std::runtime_error("binary_deserializer: invalid primary key");
      }
      return pk;
    }

  private:
    binary_deserializer &deserializer_;
    identifier expected_key_;
    byte_reader reader_;
  };

  template<class Type>
  object_ptr<Type> deserialize_object(const identifier &pk) {
    if (!pk.is_valid()) {
      throw std::invalid_argument("binary_deserializer: invalid primary key");
    }
    const key_type key{typeid(Type), pk};
    if (const auto cached = cache_.find(key); cached != cache_.end()) {
      object_ptr<Type> result(std::static_pointer_cast<Type>(cached->second));
      result.primary_key(pk);
      return result;
    }

    const auto serialized = objects_.find(key);
    if (serialized == objects_.end()) {
      throw std::out_of_range("binary_deserializer: object not found");
    }

    auto object = std::make_shared<Type>();
    cache_.emplace(key, object);
    object_ptr<Type> result(object);
    result.primary_key(pk);
    try {
      object_reader reader(*this, pk, serialized->second);
      access::process(reader, *object);
      reader.finish();
    } catch (...) {
      cache_.erase(key);
      throw;
    }
    return result;
  }

private:
  map_type objects_;
  std::map<key_type, std::shared_ptr<void>> cache_;
};
}

#endif // MATADOR_BINARY_DESERIALIZER_HPP