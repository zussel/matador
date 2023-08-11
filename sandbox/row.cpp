#include "matador/utils/any.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/value_visitor.hpp"
#include "matador/sql/types.hpp"

#include <unordered_map>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class column
{
public:
  column(std::string name, matador::database_type type, long size)
  : name_(std::move(name))
  , database_type_(type)
  , size_(size) {}

  column(std::string name, matador::database_type type, long size, matador::any val)
  : name_(std::move(name))
  , value_(std::move(val))
  , database_type_(type)
  , size_(size) {}

  template < typename Type >
  column(std::string name, const Type &val, long size)
  : name_(std::move(name))
  , value_(val)
  , database_type_(matador::data_type_traits<Type>::type())
  , size_(size) {}

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    serializer.apply(name_, value_);
  }

  const std::string& name() const {
    return name_;
  }

  matador::database_type database_type() const {
    return database_type_;
  }

  long size() const {
    return size_;
  }

  template<typename Type>
  Type as() const {
    return value_.cast<Type>();
  }

private:
  std::string name_;
  matador::any value_;
  matador::database_type database_type_;
  long size_;
};

std::shared_ptr<column> make_column(const std::string &name, matador::database_type type, long size = -1, const matador::any& val = {}) {
  return std::make_shared<column>(name, type, size, val);
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, long size = -1) {
  return make_column(name, matador::data_type_traits<Type>::type(), size, matador::any(Type()));
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const Type &val, long size = -1) {
  return std::make_shared<column>(name, val, size);
}

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

class column_printer : public matador::serializer
{
public:
  void apply(const std::string &id, matador::any &val) {
    visitor_.apply(val, id.c_str(), this);
  }

  void on_attribute(const char *id, char &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, short &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, int &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, long &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, long long int &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, unsigned char &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, unsigned short &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, unsigned int &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, unsigned long long int &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, unsigned long &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, bool &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, float &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, double &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, char *x, long size) override
  {
    print(id, x, size);
  }

  void on_attribute(const char *id, std::string &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, std::string &x, long size) override
  {
    print(id, x, size);
  }

  void on_attribute(const char *id, matador::time &x) override
  {
    print(id, x);
  }

  void on_attribute(const char *id, matador::date &x) override
  {
    print(id, x);
  }

  void on_belongs_to(const char * /*id*/, matador::identifiable_holder &/*x*/, matador::cascade_type) override { }
  void on_has_one(const char * /*id*/, matador::identifiable_holder &/*x*/, matador::cascade_type) override { }
  void on_has_many(const char * /*id*/, matador::abstract_has_many &/*x*/, const char *, const char *, matador::cascade_type) override { }
  void on_has_many(const char * /*id*/, matador::abstract_has_many &/*x*/, matador::cascade_type) override { }

private:
  template<typename Type>
  void print(const char *id, Type &val, long size = -1) {
    std::cout << "[" << id << "]: " << val << " (size: " << size << ")\n";
  }

private:
  matador::detail::value_visitor visitor_;
};

int main()
{
  row r;
  r.add_column(make_column<long>("id"));
  r.add_column(make_column<std::string>("name","george", 255));
  r.add_column(make_column<matador::date>("birthday"));

  column_printer cp;

  r.serialize(cp);

  auto col = r.at(2);
  std::cout << "col " << col->name() << " val " << col->as<matador::date>() << "\n";
  col = r.at("id");
  std::cout << "col " << col->name() << " val " << col->as<long>() << "\n";
}