#ifndef MATADOR_COLUMN_PRINTER_HPP
#define MATADOR_COLUMN_PRINTER_HPP

#include "matador/utils/any.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/value_processor.hpp"

class column_printer : public matador::serializer
{
public:
  void apply(const std::string &id, matador::any &val) {
    visitor_.apply(val, id.c_str(), *this);
  }

  void on_attribute(const char *id, char &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, short &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, int &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, long &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, long long int &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, unsigned char &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, unsigned short &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, unsigned int &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, unsigned long long int &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, unsigned long &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, bool &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, float &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, double &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, char *x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, std::string &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, matador::time &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_attribute(const char *id, matador::date &x, const matador::field_attributes &attr) override
  {
    print(id, x, attr);
  }

  void on_belongs_to(const char * /*id*/, matador::identifiable_holder &/*x*/, matador::cascade_type) override { }
  void on_has_one(const char * /*id*/, matador::identifiable_holder &/*x*/, matador::cascade_type) override { }
  void on_has_many(const char * /*id*/, matador::abstract_has_many &/*x*/, const char *, const char *, matador::cascade_type) override { }
  void on_has_many(const char * /*id*/, matador::abstract_has_many &/*x*/, matador::cascade_type) override { }

private:
  template<typename Type>
  void print(const char *id, Type &val, const matador::field_attributes &/*attr*/) {
    std::cout << "[" << id << "]: " << val << "\n";
  }

  void print(const char *id, std::string &val, const matador::field_attributes &attr) {
    std::cout << "[" << id << "]: " << val << " (size: " << attr.size() << ")\n";
  }

private:
  matador::detail::value_processor visitor_;
};

#endif //MATADOR_COLUMN_PRINTER_HPP
