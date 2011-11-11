#include "Car.hpp"
#include "Engine.hpp"

#include "object/object_atomizer.hpp"

using namespace oos;
using namespace std;

Car::Car()
{}

Car::Car(const std::string &brand, const std::string &type)
  : brand_(brand)
  , type_(type)
{}

Car::~Car()
{}

void Car::read_from(oos::object_atomizer *reader)
{
  object::read_from(reader);
  reader->read_string("brand", brand_);
  reader->read_string("type", type_);
  reader->read_object("engine", engine_);
}

void Car::write_to(oos::object_atomizer *writer) const
{
  object::write_to(writer);
  writer->write_string("brand", brand_);
  writer->write_string("type", type_);
  writer->write_object("engine", engine_);
}

std::string Car::brand() const
{
  return brand_;
}

void Car::brand(const std::string &b)
{
  mark_modified();
  brand_ = b;
}

std::string Car::type() const
{
  return type_;
}

void Car::type(const std::string &t)
{
  mark_modified();
  type_ = t;
}

oos::object_ptr<Engine> Car::engine() const
{
  return engine_;
}

void Car::engine(const oos::object_ptr<Engine> &e)
{
  mark_modified();
  engine_ = e;
}

std::ostream& operator <<(std::ostream &os, const Car &c)
{
  os << "Car [" << c.brand() << ", " << c.type() << "]";
  return os;
}
