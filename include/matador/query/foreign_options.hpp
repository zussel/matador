#ifndef MATADOR_FOREIGN_OPTIONS_HPP
#define MATADOR_FOREIGN_OPTIONS_HPP

#include "matador/query/fetch_type.hpp"
#include "matador/query/cascade_type.hpp"

// ReSharper disable CppNonExplicitConvertingConstructor
namespace matador::query {
class foreign_options {
public:
  foreign_options() = default;
  foreign_options(cascade_type cascade); // NOLINT(*-explicit-constructor)
  foreign_options(fetch_type fetch); // NOLINT(*-explicit-constructor)
  foreign_options(cascade_type cascade, fetch_type fetch);
  foreign_options(const foreign_options &x) = default;
  foreign_options& operator=(const foreign_options &x) = default;
  foreign_options(foreign_options &&x) = default;
  foreign_options& operator=(foreign_options &&x) = default;
  ~foreign_options() = default;

  [[nodiscard]] cascade_type cascade() const;
  [[nodiscard]] fetch_type fetch() const;

private:
  cascade_type cascade_{cascade_type::None};
  fetch_type fetch_{fetch_type::Lazy};
};
}

namespace matador {
const query::foreign_options CascadeNoneFetchLazy {};
const query::foreign_options CascadeNoneFetchEager {query::fetch_type::Eager};
const query::foreign_options CascadeAllFetchLazy {query::cascade_type::All, query::fetch_type::Lazy};
const query::foreign_options CascadeAllFetchEager {query::cascade_type::All, query::fetch_type::Eager};
}
#endif //MATADOR_FOREIGN_OPTIONS_HPP
