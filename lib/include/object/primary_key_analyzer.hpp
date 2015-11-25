#ifndef PRIMARY_KEY_ANALYZER_HPP
#define PRIMARY_KEY_ANALYZER_HPP

#include "object/serializer.hpp"
#include "access.hpp"

namespace oos {

class prototype_node;
class serializable;
class object_base_ptr;

/// @cond OOS_DEV

template < class T >
class primary_key_analyzer
{
public:
  primary_key_analyzer();
  virtual ~primary_key_analyzer();

  void analyze();

  template < class T >
  void serialize(const T &x) {
    oos::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char*, const T&) {}

  void serialize(const char*, const char*, size_t) {}
  void serialize(const char *id, const basic_identifier &x);

private:
  prototype_node &node_;
};

/// @endcond

}
#endif /* PRIMARY_KEY_ANALYZER_HPP */
