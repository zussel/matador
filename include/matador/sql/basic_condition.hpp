#ifndef QUERY_BASIC_CONDITION_HPP
#define QUERY_BASIC_CONDITION_HPP

#include "matador/sql/column.hpp"

#include <unordered_map>
#include <cstdint>
#include <string>

namespace matador::sql {

class dialect;
struct query_context;

class basic_condition
{
public:
  basic_condition() = default;
  virtual ~basic_condition() = default;

  enum class operand_t : uint8_t
  {
    EQUAL = 0,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    OR,
    AND,
    NOT,
    IN_LIST,
    LIKE
  };

  virtual std::string evaluate(const dialect &dialect, query_context &query) const = 0;

  static std::unordered_map<operand_t, std::string> operands;
};

class basic_column_condition : public basic_condition
{
public:
  column field_;
  std::string operand;

  basic_column_condition(column fld, basic_condition::operand_t op);
};

class basic_in_condition : public basic_condition
{
public:
  column field_;

  explicit basic_in_condition(column fld);

  [[nodiscard]] virtual size_t size() const = 0;
};

/// @endcond

}

#endif //QUERY_BASIC_CONDITION_HPP
