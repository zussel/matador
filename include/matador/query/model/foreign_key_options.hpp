#ifndef MATADOR_FOREIGN_OPTIONS_HPP
#define MATADOR_FOREIGN_OPTIONS_HPP

#include "matador/query/fetch_type.hpp"
#include "matador/query/cascade_type.hpp"

// ReSharper disable CppNonExplicitConvertingConstructor
namespace matador::query {
class foreign_key_options {
public:
  foreign_key_options() = default;
  foreign_key_options(const cascade_types& cascade); // NOLINT(*-explicit-constructor)
  foreign_key_options(fetch_type fetch); // NOLINT(*-explicit-constructor)
  foreign_key_options(const cascade_types& cascade, fetch_type fetch);
  foreign_key_options(const foreign_key_options &x) = default;
  foreign_key_options& operator=(const foreign_key_options &x) = default;
  foreign_key_options(foreign_key_options &&x) = default;
  foreign_key_options& operator=(foreign_key_options &&x) = default;
  ~foreign_key_options() = default;

  [[nodiscard]] const cascade_types& cascade() const;
  [[nodiscard]] fetch_type fetch() const;

private:
  cascade_types cascade_{cascade_type::None};
  fetch_type fetch_{fetch_type::Lazy};
};
}

namespace matador {
const query::foreign_key_options CascadeNoneFetchLazy {};
const query::foreign_key_options CascadeNoneFetchEager {query::fetch_type::Eager};
const query::foreign_key_options CascadeAllFetchLazy {query::cascade_type::All, query::fetch_type::Lazy};
const query::foreign_key_options CascadeAllFetchEager {query::cascade_type::All, query::fetch_type::Eager};
}
#endif //MATADOR_FOREIGN_OPTIONS_HPP
