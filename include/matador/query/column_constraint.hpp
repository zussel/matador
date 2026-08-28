#ifndef MATADOR_COLUMN_CONSTRAINT_HPP
#define MATADOR_COLUMN_CONSTRAINT_HPP

#include "matador/utils/flags.hpp"

namespace matador::query {
enum class column_constraint : uint8_t {
    None =        0,
    Index =       1 << 0,
    Unique =      1 << 1,
    PrimaryKey =  1 << 2,
    ForeignKey =  1 << 3,
    Identity =    1 << 4,
    Default =     1 << 5,
    NotNull =     1 << 6
  };

using column_constraints = utils::flags<column_constraint>;
}
#endif //MATADOR_COLUMN_CONSTRAINT_HPP
