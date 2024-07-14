#ifndef MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP

#include "matador/object/export.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class object_proxy;

class MATADOR_OBJECT_API basic_has_many_to_many_item
{
protected:
  basic_has_many_to_many_item() = default;
  basic_has_many_to_many_item(std::string left_column, std::string right_column);

  virtual ~basic_has_many_to_many_item() = default;

public:
  [[nodiscard]] virtual basic_has_many_to_many_item* clone() const = 0;

  [[nodiscard]] std::string left_column() const;
  [[nodiscard]] std::string right_column() const;

  void left_column(const std::string &left_column_name);
  void right_column(const std::string &right_column_name);

private:
  std::string left_column_;
  std::string right_column_;
};

/// @endcond

}
#endif //MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
