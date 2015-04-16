#ifndef OBJECT_PREPARATOR_HPP
#define OBJECT_PREPARATOR_HPP

#include "object/object_atomizer.hpp"


namespace oos {

class object;
class prototype_tree;

class object_preparator : public generic_object_reader<object_preparator>
{
public:
  explicit object_preparator(prototype_tree &tree);
  virtual ~object_preparator();

  void prepare(object *obj);

  template < class T >
  void read_value(const char*, const T&) {}

  void read_value(const char*, char*, int) {}
  void read_value(const char*, object_base_ptr &x);

private:
  prototype_tree &tree_;
};

}
#endif /* OBJECT_PREPARATOR_HPP */
