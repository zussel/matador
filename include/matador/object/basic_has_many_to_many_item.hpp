#ifndef MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP

#include <string>

namespace matador {

class object_proxy;

class basic_has_many_to_many_item
{
protected:
  basic_has_many_to_many_item(object_proxy *left_proxy, object_proxy *right_proxy,
                              const std::string &left_column, const std::string &right_column);

public:
  object_proxy* left_proxy() const;
  object_proxy* right_proxy() const;

  std::string left_column() const;
  std::string right_column() const;

private:
  std::string left_column_;
  std::string right_column_;

  object_proxy *left_proxy_;
  object_proxy *right_proxy_;
};

}
#endif //MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
