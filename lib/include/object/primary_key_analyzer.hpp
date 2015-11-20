#ifndef PRIMARY_KEY_ANALYZER_HPP
#define PRIMARY_KEY_ANALYZER_HPP

#include "serializer.hpp"

namespace oos {

class prototype_node;
class serializable;
class object_base_ptr;

/// @cond OOS_DEV

class primary_key_analyzer : public generic_serializer<primary_key_analyzer>
{
public:
  explicit primary_key_analyzer(prototype_node &node);
  virtual ~primary_key_analyzer();

  void analyze();

  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, size_t) {}
  void write_value(const char *id, const basic_identifier &x);

private:
  prototype_node &node_;
};

/// @endcond

}
#endif /* PRIMARY_KEY_ANALYZER_HPP */
