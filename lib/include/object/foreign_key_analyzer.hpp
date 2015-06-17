#ifndef FOREIGN_KEY_ANALYZER_HPP
#define FOREIGN_KEY_ANALYZER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

class prototype_node;
class serializable;
class object_base_ptr;

class foreign_key_analyzer : public generic_object_writer<foreign_key_analyzer>
{
public:
  explicit foreign_key_analyzer(prototype_node &node);
  virtual ~foreign_key_analyzer();

  void analyze();

  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, int) {}
  void write_value(const char *id, const object_base_ptr &x);

private:
  prototype_node &node_;
};

}

#endif /* FOREIGN_KEY_ANALYZER_HPP */
