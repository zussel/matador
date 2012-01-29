#include "FactoryTestUnit.hpp"

#include "tools/factory.hpp"

#include <iostream>

using namespace oos;
using std::cout;
using std::endl;

class Vehicle
{
protected:
  Vehicle(const char *t) : type_(t) {}

public:
  virtual ~Vehicle() {}

  std::string type() const { return type_; }

private:
  std::string type_;
};

class Plane : public Vehicle
{
public:
  Plane() : Vehicle("plane") {}
  virtual ~Plane() {}
};

class Bike : public Vehicle
{
public:
  Bike() : Vehicle("bike") {}
  virtual ~Bike() {}
};

class Car : public Vehicle
{
public:
  Car() : Vehicle("car") {}
  virtual ~Car() {}
};

class Ship : public Vehicle
{
public:
  Ship() : Vehicle("ship") {}
  virtual ~Ship() {}
};

FactoryTestUnit::FactoryTestUnit()
  : unit_test("factory test unit")
{
  add_test("create", std::tr1::bind(&FactoryTestUnit::create_factory, this), "create factory");
  add_test("insert", std::tr1::bind(&FactoryTestUnit::insert_items, this), "insert items into factory");
  add_test("produce", std::tr1::bind(&FactoryTestUnit::produce_items, this), "produce items with factory");
  add_test("list", std::tr1::bind(&FactoryTestUnit::list_items, this), "list all items in factory");
}

FactoryTestUnit::~FactoryTestUnit()
{}

void FactoryTestUnit::create_factory()
{
  cout << endl;
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  cout << "factory created\n";
}

void FactoryTestUnit::insert_items()
{
  cout << endl;
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  std::cout << "size of factory before: " << vfac.size() << "\n";

  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  std::cout << "size of factory after: " << vfac.size() << "\n";
}

void FactoryTestUnit::produce_items()
{
  cout << endl;
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  std::cout << "producing a car ... ";

  Vehicle *vec = vfac.create("car");

  if (vec) {
    std::cout << "success [type: " << vec->type() << "]\n";
    delete vec;
  } else {
    std::cout << "failed [null]\n";
  }
}

void FactoryTestUnit::list_items()
{
  cout << endl;
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  vehicle_factory::const_iterator first = vfac.begin();
  vehicle_factory::const_iterator last = vfac.end();

  while (first != last) {
    std::cout << "producer of type [" << (first++)->first << "]\n";
  }
}
