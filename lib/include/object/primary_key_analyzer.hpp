#include "object_atomizer.hpp"

#ifndef PRIMARY_KEY_ANALYZER_HPP
#define PRIMARY_KEY_ANALYZER_HPP

namespace oos {

class prototype_node;
class object;

class primary_key_analyzer : public generic_object_reader<primary_key_analyzer>
{
public:
  explicit primary_key_analyzer(prototype_node &node);
  virtual ~primary_key_analyzer();

  void analyze(object *obj);

  template < class T >
  void read_value(const char*, const T&) {}

  void read_value(const char*, char*, int) {}
  void read_value(const char *id, primary_key_base &x);

private:
  prototype_node &node_;
};

}
#endif /* PRIMARY_KEY_ANALYZER_HPP */
