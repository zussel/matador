#ifndef MATADOR_FOREIGN_ATTRIBUTES_HPP
#define MATADOR_FOREIGN_ATTRIBUTES_HPP

#include "matador/utils/fetch_type.hpp"
#include "matador/utils/cascade_type.hpp"

namespace matador {

class foreign_attributes
{
public:
  foreign_attributes() = default;
  foreign_attributes(cascade_type cascade); // NOLINT(*-explicit-constructor)
  foreign_attributes(fetch_type fetch); // NOLINT(*-explicit-constructor)
  foreign_attributes(cascade_type cascade, fetch_type fetch);
  foreign_attributes(const foreign_attributes &x) = default;
  foreign_attributes& operator=(const foreign_attributes &x) = default;
  foreign_attributes(foreign_attributes &&x) = default;
  foreign_attributes& operator=(foreign_attributes &&x) = default;
  ~foreign_attributes() = default;

  [[nodiscard]] cascade_type cascade() const;
  [[nodiscard]] fetch_type fetch() const;

private:
  cascade_type cascade_{cascade_type::NONE};
  fetch_type fetch_{fetch_type::LAZY};
};

const foreign_attributes default_foreign_attributes {};

}

#endif //MATADOR_FOREIGN_ATTRIBUTES_HPP
