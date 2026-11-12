#ifndef MATADOR_BINARY_SERIALIZER_HPP
#define MATADOR_BINARY_SERIALIZER_HPP

#include "matador/query/mapping/access.hpp"
#include "matador/query/model/identifier.hpp"
#include "matador/query/relations/collection.hpp"
#include "matador/query/relations/object_ptr.hpp"
#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/types.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <utility>

namespace matador::query {
namespace detail {
template<class Type, class = void>
struct is_complete_type : std::false_type {};

template<class Type>
struct is_complete_type<Type, std::void_t<decltype(sizeof(Type))>> : std::true_type {};
}

/**
 * Serializes model fields in process order, prefixing each field with its
 * length-prefixed name. Strings and blobs use a uint64 length; optional values
 * use a presence flag. Other values use their native in-memory representation,
 * so the encoding is not a portable interchange format. Relation fields store
 * primary-key references while each related object receives its own map entry.
 *
 * Each call to serialize replaces the map. A copied buffer_view owns its bytes
 * independently and remains valid after the map is cleared or replaced.
 */
class binary_serializer {
public:
  using key_type = std::pair<std::type_index, identifier>;
  using map_type = std::map<key_type, utils::buffer_view>;

  template<class Type>
  const map_type &serialize(Type &object) {
    objects_.clear();
    try {
      serialize_object(object);
    } catch (...) {
      objects_.clear();
      throw;
    }
    return objects_;
  }

  template<class Type>
  const map_type &serialize(const Type &object) {
    return serialize(const_cast<Type &>(object));
  }

  [[nodiscard]] const map_type &objects() const noexcept {
    return objects_;
  }

  [[nodiscard]] const utils::buffer_view *find(const std::type_index &type, const identifier &pk) const {
    const auto entry = objects_.find({type, pk});
    return entry == objects_.end() ? nullptr : &entry->second;
  }

  template<class Type>
  [[nodiscard]] const utils::buffer_view *find(const identifier &pk) const {
    return find(typeid(Type), pk);
  }

  void clear() noexcept {
    objects_.clear();
  }

private:
  class primary_key_reader {
  public:
    template<class Base>
    void on_base(const Base &base) {
      access::process(*this, base);
    }

    template<class Type>
    void on_primary_key(const char *, Type &pk, const primary_key_options &) {
      using value_type = std::decay_t<Type>;
      if constexpr (is_identifier_supported_v<value_type>) {
        key_ = identifier(pk);
      } else {
        throw std::invalid_argument("binary_serializer: unsupported primary key type");
      }
    }

    void on_revision(const char *, uint64_t &) {}

    template<class Type>
    void on_attribute(const char *, Type &, const column_options &) {}

    template<class Type>
    void on_belongs_to(const char *, Type &, const foreign_key_options &) {}

    template<class Type>
    void on_has_one(const char *, Type &, const char *, const foreign_key_options &) {}

    template<class Type>
    void on_has_many(const char *, Type &, const char *, const foreign_key_options &) {}

    template<class Type>
    void on_has_many(const char *, Type &, const foreign_key_options &) {}

    template<class Type>
    void on_has_many_to_many(const char *, Type &, const char *, const char *, const foreign_key_options &) {}

    template<class Type>
    void on_has_many_to_many(const char *, Type &, const foreign_key_options &) {}

    [[nodiscard]] const std::optional<identifier> &key() const noexcept {
      return key_;
    }

  private:
    std::optional<identifier> key_;
  };

  class object_writer {
  public:
    explicit object_writer(binary_serializer &serializer)
    : serializer_(serializer) {}

    template<class Base>
    void on_base(const Base &base) {
      access::process(*this, base);
    }

    template<class Type>
    void on_primary_key(const char *name, Type &pk, const primary_key_options &) {
      write_field(name);
      write_value(pk);
    }

    template<class Type>
    void on_revision(const char *name, Type &revision) {
      write_field(name);
      write_value(revision);
    }

    template<class Type>
    void on_attribute(const char *name, Type &value, const column_options &) {
      write_field(name);
      write_value(value);
    }

    template<class Type>
    void on_belongs_to(const char *name, Type &relation, const foreign_key_options &) {
      write_field(name);
      write_related_value(relation);
    }

    template<class Type>
    void on_has_one(const char *name, Type &relation, const char *, const foreign_key_options &) {
      write_field(name);
      write_related_value(relation);
    }

    template<class CollectionType>
    void on_has_many(const char *name, CollectionType &relations, const char *, const foreign_key_options &) {
      write_field(name);
      write_collection(relations);
    }

    template<class CollectionType>
    void on_has_many(const char *name, CollectionType &relations, const foreign_key_options &) {
      write_field(name);
      write_collection(relations);
    }

    template<class CollectionType>
    void on_has_many_to_many(const char *name, CollectionType &relations, const char *, const char *, const foreign_key_options &) {
      write_field(name);
      write_collection(relations);
    }

    template<class CollectionType>
    void on_has_many_to_many(const char *name, CollectionType &relations, const foreign_key_options &) {
      write_field(name);
      write_collection(relations);
    }

    [[nodiscard]] utils::buffer_view view() const {
      return buffer_.view();
    }

  private:
    void write_field(const char *name) {
      write_string(name ? std::string_view(name) : std::string_view{});
    }

    void write_string(std::string_view value) {
      const auto length = static_cast<std::uint64_t>(value.size());
      buffer_.write(length);
      buffer_.append(value);
    }

    void write_identifier(const identifier &pk) {
      const auto type = static_cast<std::uint8_t>(pk.type());
      buffer_.write(type);
      write_string(pk.str());
    }

    template<class Type>
    void write_value(const std::optional<Type> &value) {
      const bool present = value.has_value();
      buffer_.write(present);
      if (present) {
        write_value(*value);
      }
    }

    void write_value(const std::string &value) {
      write_string(value);
    }

    void write_value(std::string_view value) {
      write_string(value);
    }

    void write_value(const utils::blob_type_t &value) {
      const auto length = static_cast<std::uint64_t>(value.size());
      buffer_.write(length);
      buffer_.append(value.data(), value.size());
    }

    template<class Type>
    void write_value(const Type &value) {
      static_assert(std::is_trivially_copyable_v<Type>, "binary_serializer supports only trivially copyable attributes, strings, blobs, and optionals");
      buffer_.write(value);
    }

    template<class Type>
    void write_related_value(Type &relation) {
      using value_type = std::decay_t<Type>;
      if constexpr (is_object_ptr<value_type>::value) {
        write_reference(relation);
      } else if constexpr (std::is_same_v<value_type, std::string> ||
                           std::is_same_v<value_type, utils::blob_type_t> ||
                           std::is_trivially_copyable_v<value_type>) {
        write_value(relation);
      } else {
        const auto pk = serializer_.primary_key_of(relation);
        write_identifier(pk);
        serializer_.serialize_object(relation);
      }
    }

    template<class Type>
    void write_reference(object_ptr<Type> &relation) {
      const bool present = !relation.empty();
      buffer_.write(present);
      if (!present) {
        return;
      }

      if constexpr (!detail::is_complete_type<Type>::value) {
        throw std::runtime_error("binary_serializer: relation target type is incomplete");
      } else {
        Type *object = relation.get();
        if (!object) {
          throw std::runtime_error("binary_serializer: unable to resolve relation");
        }
        const auto pk = serializer_.primary_key_of(*object);
        write_identifier(pk);
        serializer_.serialize_object(*object);
      }
    }

    template<class CollectionType>
    void write_collection(CollectionType &relations) {
      const auto count = static_cast<std::uint64_t>(relations.size());
      buffer_.write(count);
      for (auto &relation : relations) {
        write_related_value(relation);
      }
    }

  private:
    binary_serializer &serializer_;
    utils::byte_buffer buffer_;
  };

  template<class Type>
  identifier primary_key_of(Type &object) {
    primary_key_reader reader;
    access::process(reader, object);
    if (!reader.key() || !reader.key()->is_valid()) {
      throw std::invalid_argument("binary_serializer: object has no valid primary key");
    }
    return *reader.key();
  }

  template<class Type>
  void serialize_object(Type &object) {
    const identifier pk = primary_key_of(object);
    const key_type key{typeid(Type), pk};
    const auto [entry, inserted] = objects_.try_emplace(key);
    if (!inserted) {
      return;
    }

    object_writer writer(*this);
    access::process(writer, object);
    entry->second = writer.view();
  }

private:
  map_type objects_;
};
}

#endif // MATADOR_BINARY_SERIALIZER_HPP