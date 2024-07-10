#ifndef MATADOR_OBJECT_DESCRIPTION_HPP
#define MATADOR_OBJECT_DESCRIPTION_HPP

#include "matador/utils/data_types.hpp"

namespace matador {

struct column_description
{
  column_description(const char *name, data_type type, size_t size)
  : name(name), type(type), size(size) {}

  std::string name;
  data_type type{data_type::type_unknown};
  size_t size{};
};

class object_description
{
public:
  object_description(std::string name, std::vector<column_description> columns)
  : name_(std::move(name)), columns_(std::move(columns)) {}

  [[nodiscard]] const std::string& name() const { return name_; }
  [[nodiscard]] const std::vector<column_description>& columns() const { return columns_; }

private:
  std::string name_;
  std::vector<column_description> columns_;
};

}
#endif //MATADOR_OBJECT_DESCRIPTION_HPP
