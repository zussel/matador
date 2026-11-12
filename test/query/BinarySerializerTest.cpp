#include <catch2/catch_test_macros.hpp>

#include "matador/query/mapping/binary_deserializer.hpp"
#include "matador/query/mapping/binary_serializer.hpp"

#include "models/author.hpp"
#include "models/recipe.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace {
struct binary_data {
  std::uint32_t id{};
  std::string text;
  std::optional<std::uint16_t> year;
  matador::utils::blob_type_t data;

  template<class Operator>
  void process(Operator &op) {
    matador::query::access::primary_key(op, "id", id);
    matador::query::access::attribute(op, "text", text);
    matador::query::access::attribute(op, "year", year);
    matador::query::access::attribute(op, "data", data);
  }
};

struct string_target;

struct string_owner {
  std::uint32_t id{};
  matador::query::object_ptr<string_target> target;

  template<class Operator>
  void process(Operator &op) {
    matador::query::access::primary_key(op, "id", id);
    matador::query::access::belongs_to(op, "target", target);
  }
};

struct string_target {
  std::string id;
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    matador::query::access::primary_key(op, "id", id);
    matador::query::access::attribute(op, "name", name);
  }
};
}

TEST_CASE("Binary serializer stores large attributes and owning views", "[query][binary_serializer]") {
  matador::query::binary_serializer serializer;
  binary_data data;
  data.id = 1;
  data.text.assign(matador::utils::byte_buffer::CHUNK_SIZE + 13, 'x');
  data.data.resize(matador::utils::byte_buffer::CHUNK_SIZE + 19);
  for (std::size_t i = 0; i < data.data.size(); ++i) {
    data.data[i] = static_cast<unsigned char>(i % 251);
  }

  serializer.serialize(data);
  const auto *serialized = serializer.find<binary_data>(matador::query::identifier(1U));
  REQUIRE(serialized != nullptr);
  REQUIRE(serialized->size() > data.text.size() + data.data.size());

  const auto snapshot = *serialized;
  const std::string serialized_bytes(snapshot.data(), snapshot.size());
  REQUIRE(serialized_bytes.find(data.text) != std::string::npos);
  const std::string blob_bytes(reinterpret_cast<const char *>(data.data.data()), data.data.size());
  REQUIRE(serialized_bytes.find(blob_bytes) != std::string::npos);

  const auto absent_optional_size = snapshot.size();
  data.year = 2026;
  serializer.serialize(data);
  const auto *with_optional = serializer.find<binary_data>(matador::query::identifier(1U));
  REQUIRE(with_optional != nullptr);
  REQUIRE(with_optional->size() == absent_optional_size + sizeof(std::uint16_t));

  data.id = 0;
  REQUIRE_THROWS_AS(serializer.serialize(data), std::invalid_argument);
  REQUIRE(serializer.objects().empty());
  REQUIRE(snapshot.size() == serialized_bytes.size());
  REQUIRE(std::string(snapshot.data(), snapshot.size()) == serialized_bytes);
}

TEST_CASE("Binary serializer stores related objects once by type and primary key", "[query][binary_serializer]") {
  matador::query::binary_serializer serializer;
  auto recipe = matador::query::make_object<matador::test::recipe>(7, "soup");
  auto ingredient = matador::query::make_object<matador::test::ingredient>(7, "salt");
  recipe->ingredients.push_back(ingredient);
  ingredient->recipes.push_back(recipe);

  const auto &objects = serializer.serialize(*recipe);

  REQUIRE(objects.size() == 2);
  const auto *recipe_view = serializer.find<matador::test::recipe>(matador::query::identifier(7U));
  const auto *ingredient_view = serializer.find<matador::test::ingredient>(matador::query::identifier(7U));
  REQUIRE(recipe_view != nullptr);
  REQUIRE(ingredient_view != nullptr);
  REQUIRE(recipe_view->as_string_view().find("ingredients") != std::string_view::npos);
  REQUIRE(ingredient_view->as_string_view().find("recipe_ingredients") != std::string_view::npos);
}

TEST_CASE("Binary serializer handles empty relations", "[query][binary_serializer]") {
  matador::query::binary_serializer serializer;
  matador::test::author author{3, "Ada", "Lovelace", "1815-12-10", 1815, true};

  serializer.serialize(author);

  REQUIRE(serializer.objects().size() == 1);
  REQUIRE(serializer.find<matador::test::author>(matador::query::identifier(3U)) != nullptr);
}

TEST_CASE("Binary deserializer restores attributes and optional values", "[query][binary_deserializer]") {
  matador::query::binary_serializer serializer;
  binary_data original;
  original.id = 42;
  original.text.assign(matador::utils::byte_buffer::CHUNK_SIZE + 5, 'z');
  original.year = 2026;
  original.data = {0, 1, 2, 127, 255};
  serializer.serialize(original);

  matador::query::binary_deserializer deserializer(serializer);
  auto restored = deserializer.deserialize<binary_data>(matador::query::identifier(42U));

  REQUIRE(restored->id == original.id);
  REQUIRE(restored->text == original.text);
  REQUIRE(restored->year == original.year);
  REQUIRE(restored->data == original.data);
  REQUIRE(restored.primary_key() == matador::query::identifier(42U));
}

TEST_CASE("Binary deserializer preserves typed identity through cyclic relations", "[query][binary_deserializer]") {
  matador::query::binary_serializer serializer;
  auto recipe = matador::query::make_object<matador::test::recipe>(7, "soup");
  auto ingredient = matador::query::make_object<matador::test::ingredient>(7, "salt");
  recipe->ingredients.push_back(ingredient);
  ingredient->recipes.push_back(recipe);
  serializer.serialize(*recipe);

  matador::query::binary_deserializer deserializer(serializer);
  auto restored = deserializer.deserialize<matador::test::recipe>(matador::query::identifier(7U));

  REQUIRE(restored->name == "soup");
  REQUIRE(restored->ingredients.size() == 1);
  auto restored_ingredient = *restored->ingredients.begin();
  REQUIRE(restored_ingredient->name == "salt");
  REQUIRE(restored_ingredient->recipes.size() == 1);
  auto restored_recipe = *restored_ingredient->recipes.begin();
  REQUIRE(restored_recipe.get() == restored.get());
  REQUIRE(restored_recipe.primary_key() == restored.primary_key());
}

TEST_CASE("Binary deserializer resolves string primary keys in relations", "[query][binary_deserializer]") {
  matador::query::binary_serializer serializer;
  string_owner owner;
  owner.id = 1;
  owner.target = matador::query::make_object<string_target>();
  owner.target->id = "target-key";
  owner.target->name = "named target";
  serializer.serialize(owner);

  matador::query::binary_deserializer deserializer(serializer);
  auto restored = deserializer.deserialize<string_owner>(matador::query::identifier(1U));

  REQUIRE(restored->target->id == "target-key");
  REQUIRE(restored->target->name == "named target");
}

TEST_CASE("Binary deserializer restores empty collections and rejects missing or malformed instances", "[query][binary_deserializer]") {
  matador::query::binary_serializer serializer;
  matador::test::author original{3, "Ada", "Lovelace", "1815-12-10", 1815, true};
  serializer.serialize(original);

  matador::query::binary_deserializer deserializer(serializer);
  auto restored = deserializer.deserialize<matador::test::author>(matador::query::identifier(3U));
  REQUIRE(restored->first_name == "Ada");
  REQUIRE(restored->books.empty());
  REQUIRE_THROWS_AS(deserializer.deserialize<matador::test::author>(matador::query::identifier(4U)), std::out_of_range);

  matador::query::binary_deserializer::map_type malformed;
  malformed.emplace(
    matador::query::binary_deserializer::key_type{typeid(binary_data), matador::query::identifier(9U)},
    matador::utils::buffer_view{});
  matador::query::binary_deserializer invalid_data(malformed);
  REQUIRE_THROWS_AS(invalid_data.deserialize<binary_data>(matador::query::identifier(9U)), std::runtime_error);

  matador::query::binary_serializer valid_serializer;
  binary_data valid_object;
  valid_object.id = 8;
  valid_serializer.serialize(valid_object);
  matador::query::binary_deserializer::map_type mismatched;
  mismatched.emplace(
    matador::query::binary_deserializer::key_type{typeid(binary_data), matador::query::identifier(9U)},
    *valid_serializer.find<binary_data>(matador::query::identifier(8U)));
  matador::query::binary_deserializer invalid_key(mismatched);
  REQUIRE_THROWS_AS(invalid_key.deserialize<binary_data>(matador::query::identifier(9U)), std::runtime_error);
}