#ifndef QUERY_KEY_VALUE_PAIR_HPP
#define QUERY_KEY_VALUE_PAIR_HPP

#include "matador/sql/column.hpp"

#include "matador/utils/types.hpp"

namespace matador::sql {

class key_value_pair
{
public:
  key_value_pair(const sql::column &col, utils::any_type value);
  key_value_pair(std::string name, utils::any_type value);
  key_value_pair(const char *name, utils::any_type value);

  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] const utils::any_type& value() const;

private:
  std::string name_;
  utils::any_type value_;
};

}
#endif //QUERY_KEY_VALUE_PAIR_HPP
