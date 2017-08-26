#ifndef MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP

#include <string>

namespace matador {

class object_proxy;

class basic_has_many_to_many_item
{
protected:
  basic_has_many_to_many_item() = default;
  basic_has_many_to_many_item(const std::string &left_column, const std::string &right_column);

public:
  std::string left_column() const;
  std::string right_column() const;

private:
  std::string left_column_;
  std::string right_column_;
};

}
#endif //MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
