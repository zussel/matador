#ifndef ATTRIBUTE_COUNTER_HPP
#define ATTRIBUTE_COUNTER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

class attribute_counter : public generic_object_writer<attribute_counter>
{
public:
  attribute_counter()
    : generic_object_writer<attribute_counter>(this)
    , count_(0)
    , with_container_(false)
  {}
  virtual ~attribute_counter() {}
  
  unsigned long count(object *o, bool with_container = false)
  {
    count_ = 0;
    with_container_ = with_container;
    
    o->serialize(*this);
    
    return count_;
  }
  
  template < class T >
  void write_value(const char*, const T&)
  {
    ++count_;
  }
  
  void write_value(const char*, const char*, int)
  {
    ++count_;
  }
  
  void write_value(const char*, const object_container&)
  {
    if (with_container_) {
      ++count_;
    }
  }

private:
  unsigned long count_;
  bool with_container_;
};

}

#endif /* ATTRIBUTE_COUNTER_HPP */
