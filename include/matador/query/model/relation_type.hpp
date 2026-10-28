#ifndef MATADOR_RELATION_TYPE_HPP
#define MATADOR_RELATION_TYPE_HPP

#include "matador/utils/enum_mapper.hpp"

#include <cstdint>

namespace matador::query {
enum class relation_type : uint8_t {
  BelongsTo,
  HasOne,
  HasMany
};

static const utils::enum_mapper<relation_type> relation_type_enum({
                                                                    { relation_type::BelongsTo, "belongs_to" },
                                                                    { relation_type::HasOne, "has_one" },
                                                                    { relation_type::HasMany, "has_many" },
                                                                  });
}
#endif //MATADOR_RELATION_TYPE_HPP
