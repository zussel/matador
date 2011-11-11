#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include <ostream>

class Engine : public oos::object
{
public:
  Engine();
  explicit Engine(unsigned int power, unsigned int cylinder, float capacity);
  virtual ~Engine();
  
  virtual void read_from(oos::object_atomizer *reader);
  virtual void write_to(oos::object_atomizer *writer) const;
  
  unsigned int power() const;
  void power(unsigned int p);
  
  unsigned int cylinder() const;
  void cylinder(unsigned int c);
  
  float capacity() const;
  void capacity(float c);
  
  friend std::ostream& operator <<(std::ostream &os, const Engine &e);

private:
  unsigned int power_;
  unsigned int cylinder_;
  float capacity_;
};

#endif /* ENGINE_HPP */
