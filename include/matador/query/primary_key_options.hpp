#ifndef primary_key_options_HPP
#define primary_key_options_HPP

#include "matador/query/primary_key_generator_type.hpp"

#include <cstddef>

namespace matador::query {
class primary_key_options final {
public:
  /**
   * Creates a primary_key_options instance
   * with size 0 (zero).
   */
  primary_key_options() = default;
  /**
   * Creates a primary_key_options instance with a
   * given size.
   *
   * @param size Size of the attribute
   */
  primary_key_options(size_t size); // NOLINT(*-explicit-constructor)
  primary_key_options(generator_type generator); // NOLINT(*-explicit-constructor)
  primary_key_options(size_t size, generator_type generator);
  ~primary_key_options() = default;
  primary_key_options(const primary_key_options &) = default;
  primary_key_options(primary_key_options &&) = default;
  primary_key_options &operator=(const primary_key_options &) = default;
  primary_key_options &operator=(primary_key_options &&) = default;
  primary_key_options& operator=(size_t size);
  primary_key_options& operator=(generator_type generator);

  /**
   * Returns the size of the field
   *
   * @return Size of the field
   */
  [[nodiscard]] size_t size() const;
  /**
   * Returns the generator type of the field
   *
   * @return Generator type of the field
   */
  [[nodiscard]] generator_type generator() const;
private:
  size_t size_ = 0;
  generator_type generator_ = generator_type::Manual;
};

const primary_key_options DefaultPkAttributes {};
const primary_key_options Manual {generator_type::Manual};
const primary_key_options Identity {generator_type::Identity};
const primary_key_options Sequence {generator_type::Sequence};
const primary_key_options Table {generator_type::Table};
const primary_key_options ManualVarChar63 {63};
const primary_key_options ManualVarChar127 {127};
const primary_key_options ManualVarChar255 {255};
const primary_key_options ManualVarChar511 {511};
const primary_key_options ManualVarChar1023 {1023};
const primary_key_options ManualVarChar2047 {2047};
const primary_key_options ManualVarChar4095 {4095};

}
#endif //primary_key_options_HPP
