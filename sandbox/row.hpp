#ifndef MATADOR_ROW_HPP
#define MATADOR_ROW_HPP

#include "column.hpp"

namespace matador {

class row
{
public:
  using column_ptr = std::shared_ptr<column>;

public:
  template < typename Type >
  void add_column(const std::string &name, long size = -1) {
    add_column(make_column<Type>(name, size));
  }

  void add_column(const std::string &name, matador::database_type type, long size = -1) {
    add_column(make_column(name, type, size));
  }

  void add_column(const column_ptr &col) {
    columns_.push_back(col);
    columns_by_name_.insert({col->name(), col});
  }

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    for (auto& col : columns_) {
      col->serialize(serializer);
    }
  }

  column_ptr at(const std::string &name) const {
    return columns_by_name_.at(name);
  }

  column_ptr at(size_t index) {
    return columns_.at(index);
  }

private:
  using column_by_index = std::vector<column_ptr>;
  using column_by_name_map = std::unordered_map<std::string, column_ptr >;

  column_by_index columns_;
  column_by_name_map columns_by_name_;
};

}
#endif //MATADOR_ROW_HPP
