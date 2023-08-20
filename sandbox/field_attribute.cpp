#include "field_attributes.hpp"

#include <iostream>

struct address {
  unsigned long id {};
};

struct person
{
  unsigned long id;
  std::string name;
  unsigned short height;
  unsigned short age;
  address home;

  template<class Operator>
  void process(Operator &op) {
    op.on_primary_key("id", id);
    op.on_attribute("name", name, { 255, matador::constraints::UNIQUE | matador::constraints::NOT_NULL });
    op.on_attribute("height", height, matador::constraints::NOT_NULL );
    op.on_attribute("age", age);
    op.on_has_one("home", home);
  }
};

struct attribute_printer
{
  template<class PkType>
  void on_primary_key(const char *id, PkType &pk, matador::field_attributes attr = matador::constraints::PRIMARY_KEY ) {
    std::cout << "field " << id << " pk " << pk << " (" << attr << ")\n";
  }

  template<class AttributeType>
  void on_attribute(const char *id, AttributeType &field, matador::field_attributes attr = {}) {
    std::cout << "field " << id << " attribute " << field << " (" << attr << ")\n";
  }

  template<class ForeignType>
  void on_has_one(const char *id, ForeignType &foreign, matador::field_attributes attr = matador::constraints::FOREIGN_KEY) {
    std::cout << "field " << id << " fk " << foreign.id << " (" << attr << ")\n";
  }
};

int main()
{
  person george { 1, "george", 187, 80 };

  attribute_printer ap;

  george.process(ap);
}