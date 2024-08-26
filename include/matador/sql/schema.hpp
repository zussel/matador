#ifndef QUERY_SCHEMA_HPP
#define QUERY_SCHEMA_HPP

#include "matador/sql/column_definition_generator.hpp"
#include "matador/sql/table_definition.hpp"

#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace matador::sql {

class connection;

enum class error {
  Error = 1
};

struct table_info
{
  std::string name;
  table_definition prototype;
};

class schema
{
public:
  using repository = std::unordered_map<std::type_index, table_info>;
  using repository_by_name = std::unordered_map<std::string, std::reference_wrapper<table_info>>;
  using iterator = repository::iterator;
  using const_iterator = repository::const_iterator;

  schema() = delete;
  explicit schema(std::string name);
  schema(const schema&) = delete;
  schema& operator=(const schema&) = delete;
  schema(schema&&) noexcept = default;
  schema& operator=(schema&&) noexcept = default;

  [[nodiscard]] std::string name() const;

  template<typename Type>
  const table_info& attach(const std::string &table_name)
  {
    return attach(std::type_index(typeid(Type)), table_info{table_name, table_definition{column_definition_generator::generate<Type>(*this)}});
  }

  const table_info& attach(std::type_index ti, const table_info& table);

  utils::result<void, error> create(connection& db);
  utils::result<void, error> drop(connection& db);

  template<typename Type>
  [[nodiscard]] std::optional<table_info> info() const
  {
    return info(std::type_index(typeid(Type)));
  }

  [[nodiscard]] std::optional<table_info> info(std::type_index ti) const;
  [[nodiscard]] std::optional<table_info> info(const std::string &name) const;

  template<typename Type>
  [[nodiscard]] std::pair<std::string, std::string> reference() const
  {
    return reference(std::type_index(typeid(Type)));
  }

  [[nodiscard]] std::pair<std::string, std::string> reference(const std::type_index &ti) const;

  template<typename Type>
  [[nodiscard]] bool exists() const
  {
    return exists(std::type_index(typeid(Type)));
  }

  [[nodiscard]] bool exists(const std::type_index &ti) const;

  iterator begin();
  [[nodiscard]] const_iterator begin() const;
  iterator end();
  [[nodiscard]] const_iterator end() const;

  [[nodiscard]] bool empty() const;

private:
  std::string name_;
  repository repository_;
  repository_by_name repository_by_name_;
};

}

#endif //QUERY_SCHEMA_HPP
