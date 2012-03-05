#include "Car.hpp"
#include "Engine.hpp"

#include "object/object_atomizer.hpp"

using namespace oos;
using namespace std;

Car::Car()
{}

Car::Car(const std::string &brand, const std::string &type, const std::string &desc)
  : brand_(brand)
  , type_(type)
  , desc_(desc)
{}

Car::~Car()
{}

void Car::read_from(oos::object_atomizer *reader)
{
  object::read_from(reader);
  reader->read_varchar("brand", brand_);
  reader->read_varchar("type", type_);
  reader->read_string("description", desc_);
  reader->read_object("engine", engine_);
}

void Car::write_to(oos::object_atomizer *writer) const
{
  object::write_to(writer);
  writer->write_varchar("brand", brand_);
  writer->write_varchar("type", type_);
  writer->write_string("description", desc_);
  writer->write_object("engine", engine_);
}

std::string Car::brand() const
{
  return brand_.str();
}

void Car::brand(const std::string &b)
{
  modify(brand_, b);
}

std::string Car::type() const
{
  return type_.str();
}

void Car::type(const std::string &t)
{
  mark_modified();
  type_ = t;
}

std::string Car::description() const
{
  return desc_;
}

void Car::description(const std::string &desc)
{
  mark_modified();
  desc_ = desc;
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
