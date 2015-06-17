#ifndef ATTRIBUTE_COUNTER_HPP
#define ATTRIBUTE_COUNTER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

/**
 * @class attribute_counter
 * @brief Counts the number of attributes within an serializable.
 * 
 * This class counts all attribute for the given
 * serializable. With a flag one can indicate that also
 * container attributes are considered for the
 * counting. The default is false.
 */
class attribute_counter : public generic_object_writer<attribute_counter>
{
public:
  /**
   * Creates a new attribute counter.
   */
  attribute_counter()
    : generic_object_writer<attribute_counter>(this)
    , count_(0)
    , with_container_(false)
  {}
  virtual ~attribute_counter() {}
  
  /**
   * Counts all attributes for the
   * given serializable. If the flag is set to true
   * all container attributes are also
   * considered for the counting.
   * 
   * @param o The serializable.
   * @param with_container Indicates wether
   *        container are considered or not.
   * @return The number of attributes.
   */
  unsigned long count(object *o, bool with_container = false)
  {
    count_ = 0;
    with_container_ = with_container;
    
    o->serialize(*this);
    
    return count_;
  }

protected:

/// @cond OOS_DEV
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
/// @endcond

private:
  unsigned long count_;
  bool with_container_;
};

}

#endif /* ATTRIBUTE_COUNTER_HPP */
