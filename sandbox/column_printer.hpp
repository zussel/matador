#ifndef MATADOR_COLUMN_PRINTER_HPP
#define MATADOR_COLUMN_PRINTER_HPP

#include "matador/utils/any.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/value_visitor.hpp"

class column_printer : public matador::serializer
{
public:
  void apply(const std::string &id, matador::any &val) {
    visitor_.apply(val, id.c_str(), *this);
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

#endif //MATADOR_COLUMN_PRINTER_HPP
