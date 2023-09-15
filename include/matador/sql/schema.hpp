#ifndef MATADOR_SCHEMA_HPP
#define MATADOR_SCHEMA_HPP

#include <stdexcept>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace matador {

class schema
{
public:
  struct table_info;
  using table_info_ptr = std::shared_ptr<table_info>;
  struct table_info
  {
    table_info(std::string n, const std::type_index &ti, table_info_ptr pi)
    : name(std::move(n)), type(ti), parent_info(std::move(pi))
    {}
    std::string name;
    std::type_index type;
    table_info_ptr parent_info;
  };

public:
  template<class Type>
  void attach(const char *type_name, const char *parent_type_name = nullptr)
  {
    table_info_ptr parent_info;
    if (parent_type_name != nullptr) {
      auto it = table_info_map_by_name_.find(parent_type_name);
      if (it == table_info_map_by_name_.end()) {
        throw std::logic_error("unknown parent " + std::string(parent_type_name));
      }
      parent_info = it->second;
    }
    internal_attach<Type>(type_name, parent_info);
  }
  
  template<class Type, class ParentType>
  void attach(const char *type_name) {
    std::type_index ti(typeid(ParentType));
    auto it = table_info_map_by_type_.find(ti);
    if (it == table_info_map_by_type_.end()) {
      throw std::logic_error("unknown parent " + std::string(ti.name()));
    }
    internal_attach<Type>(type_name, it->second);
  }

  template<class Type>
  table_info_ptr find() const{
    const auto it = table_info_map_by_type_.find(std::type_index(typeid(Type)));
    if (it == table_info_map_by_type_.end()) {
      return {};
    }
    return it->second;
  }

  table_info_ptr find(const char *type_name) const{
    const auto it = table_info_map_by_name_.find(type_name);
    if (it == table_info_map_by_name_.end()) {
      return {};
    }
    return it->second;
  }

private:
  using table_info_map_by_index = std::unordered_map<std::type_index, table_info_ptr>;
  using table_info_map_by_name = std::unordered_map<std::string, table_info_ptr>;

private:
  template<class Type>
  void internal_attach(const char *type_name, const table_info_ptr &parent_info)
  {
    const auto ti = std::type_index(typeid(Type));
    const auto it = table_info_map_by_type_.find(ti);
    if (it == table_info_map_by_type_.end()) {
      table_info_map_by_type_.insert({ti, std::make_shared<table_info>(type_name, ti, parent_info)});
    } else {
      throw std::logic_error("type " + it->second->name + " already attached to schema");
    }
  }

  table_info_map_by_index table_info_map_by_type_;
  table_info_map_by_name table_info_map_by_name_;
};

}
#endif //MATADOR_SCHEMA_HPP
