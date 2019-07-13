#include "FactoryTestUnit.hpp"

#include "matador/utils/factory.hpp"

#include <iostream>

using namespace matador;

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
  : unit_test("factory", "factory test unit")
{
  add_test("create", std::bind(&FactoryTestUnit::create_factory, this), "create factory");
  add_test("insert", std::bind(&FactoryTestUnit::insert_items, this), "insert items into factory");
  add_test("produce", std::bind(&FactoryTestUnit::produce_items, this), "produce items with factory");
  add_test("list", std::bind(&FactoryTestUnit::list_items, this), "list all items in factory");
}

void FactoryTestUnit::create_factory()
{
  typedef factory<std::string, Vehicle> vehicle_factory_t;
  auto *vfac = new vehicle_factory_t;

  UNIT_ASSERT_NOT_NULL(vfac);
  UNIT_ASSERT_TRUE(vfac->empty());
  UNIT_ASSERT_EQUAL((int)vfac->size(), 0);
  
  delete vfac;
}

void FactoryTestUnit::insert_items()
{
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  UNIT_ASSERT_TRUE(vfac.empty());
  UNIT_ASSERT_EQUAL((int)vfac.size(), 0);


  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  UNIT_ASSERT_FALSE(vfac.empty());
  UNIT_ASSERT_EQUAL((int)vfac.size(), 4);
}

void FactoryTestUnit::produce_items()
{
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  Vehicle *vec = vfac.create("car");

  UNIT_ASSERT_NOT_NULL(vec);
  UNIT_ASSERT_EQUAL(vec->type(), "car");
  
  delete vec;
}

void FactoryTestUnit::list_items()
{
  typedef factory<std::string, Vehicle> vehicle_factory;
  vehicle_factory vfac;

  vfac.insert("plane", new vehicle_factory::default_producer<Plane>);
  vfac.insert("bike", new vehicle_factory::default_producer<Bike>);
  vfac.insert("car", new vehicle_factory::default_producer<Car>);
  vfac.insert("ship", new vehicle_factory::default_producer<Ship>);

  vehicle_factory::const_iterator first = vfac.begin();
  vehicle_factory::const_iterator last = vfac.end();

  UNIT_ASSERT_FALSE(first == last);
  
  UNIT_ASSERT_EQUAL(vfac.size(), (size_t)4);
}
