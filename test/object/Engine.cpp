#include "Engine.hpp"

#include "object/object_atomizer.hpp"

using namespace oos;

Engine::Engine()
  : cylinder_(0)
  , capacity_(0.0)
{}

Engine::Engine(unsigned int power, unsigned int cylinder, float capacity)
  : power_(power)
  , cylinder_(cylinder)
  , capacity_(capacity)
{}

Engine::~Engine()
{}

void Engine::read_from(oos::object_atomizer *reader)
{
  object::read_from(reader);
  reader->read_unsigned("power", power_);
  reader->read_unsigned("cylinder", cylinder_);
  reader->read_float("capacity", capacity_);
}

void Engine::write_to(oos::object_atomizer *writer) const
{
  object::write_to(writer);
  writer->write_unsigned("power", power_);
  writer->write_unsigned("cylinder", cylinder_);
  writer->write_float("capacity", capacity_);
}

unsigned int Engine::power() const
{
  return power_;
}

void Engine::power(unsigned int p)
{
  mark_modified();
  power_ = p;
}

unsigned int Engine::cylinder() const
{
  return cylinder_;
}

void Engine::cylinder(unsigned int c)
{
  mark_modified();
  cylinder_ = c;
}

float Engine::capacity() const
{
  return capacity_;
}

void Engine::capacity(float c)
{
  mark_modified();
  capacity_ = c;
}

std::ostream& operator <<(std::ostream &os, const Engine &e)
{
  os << "Engine [" << e.power() << "KW, " << e.cylinder() << "cylinder, " << e.capacity() << "qcm]";
  return os;
}
