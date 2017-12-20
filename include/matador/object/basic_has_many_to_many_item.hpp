#ifndef MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_BASIC_HAS_MANY_TO_MANY_ITEM_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
#define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#else
#define MATADOR_OBJECT_API
#endif

#include <string>

namespace matador {

class object_proxy;

class MATADOR_OBJECT_API basic_has_many_to_many_item
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
