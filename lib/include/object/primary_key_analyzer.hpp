#ifndef PRIMARY_KEY_ANALYZER_HPP
#define PRIMARY_KEY_ANALYZER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

class prototype_node;
class serializable;
class object_base_ptr;

class primary_key_analyzer : public generic_object_writer<primary_key_analyzer>
{
public:
  explicit primary_key_analyzer(prototype_node &node);
  virtual ~primary_key_analyzer();

  void analyze();

  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, int) {}
  void write_value(const char *id, const primary_key_base &x);

private:
  prototype_node &node_;
};

}
#endif /* PRIMARY_KEY_ANALYZER_HPP */
