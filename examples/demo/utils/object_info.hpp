#ifndef MATADOR_OBJECT_INFO_HPP
#define MATADOR_OBJECT_INFO_HPP

#include "matador/object/object_store.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/data_types.hpp"

struct column_description
{
  column_description(const char *name, matador::data_type type, size_t size)
  : name(name), type(type), size(size) {}

  std::string name;
  matador::data_type type{matador::data_type::type_unknown};
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

class object_info
{
public:
  template<typename Type>
  object_description describe(const matador::object_store &store)
  {
    Type obj;
    columns_.clear();
    matador::access::process(*this, obj);

    const auto it = store.find<Type>();

    return {it->type(), columns_};
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    columns_.emplace_back(id, matador::sql::data_type_traits<ValueType>::type(0), 0);
  }
  void on_primary_key(const char *id, std::string &, size_t size)
  {
    columns_.emplace_back(id, matador::data_type::type_varchar, size);
  }
  void on_revision(const char *id, unsigned long long &/*rev*/)
  {
    columns_.emplace_back(id, matador::data_type::type_varchar, 0);
  }

  template<typename Type>
  void on_attribute(const char *id, Type &/*x*/, const matador::utils::field_attributes &attr = matador::utils::null_attributes)
  {
    columns_.emplace_back(id, matador::sql::data_type_traits<Type>::type(attr.size()), attr.size());
  }
  void on_attribute(const char *id, char * /*x*/, const matador::utils::field_attributes &attr = matador::utils::null_attributes)
  {
    columns_.emplace_back(id, matador::sql::data_type_traits<const char *>::type(attr.size()), attr.size());
  }

  void on_belongs_to(const char *id, matador::identifiable_holder &x, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, x.create_identifier().type(), 0);
  }

  void on_has_one(const char *id, matador::identifiable_holder &x, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, x.create_identifier().type(), 0);
  }

  void on_has_many(const char *id, matador::abstract_container&, const char * /*join_column*/, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, matador::data_type::type_unknown, 0);
  }
  void on_has_many(const char *id, matador::abstract_container&, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, matador::data_type::type_unknown, 0);
  }
  void on_has_many_to_many(const char *id, matador::abstract_container&, const char * /*join_column*/, const char * /*inverse_join_column*/, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, matador::data_type::type_unknown, 0);
  }
  void on_has_many_to_many(const char *id, matador::abstract_container&, const matador::utils::foreign_attributes &/*attr*/ = matador::utils::default_foreign_attributes)
  {
    columns_.emplace_back(id, matador::data_type::type_unknown, 0);
  }

private:
  std::vector<column_description> columns_;
};

#endif //MATADOR_OBJECT_INFO_HPP
