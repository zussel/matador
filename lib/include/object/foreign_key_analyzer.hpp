#ifndef FOREIGN_KEY_ANALYZER_HPP
#define FOREIGN_KEY_ANALYZER_HPP

#include "object/access.hpp"

#include <cstddef>

namespace oos {

class object_base_ptr;
class prototype_node;

/// @cond OOS_DEV

class foreign_key_analyzer
{
public:
  explicit foreign_key_analyzer(prototype_node &node);
  ~foreign_key_analyzer();

  template < class T >
  static void analyze(prototype_node &node)
  {
    foreign_key_analyzer analyzer(node);
    T obj;
    oos::access::serialize(analyzer, obj);
  }

  template < class V >
  void serialize(const char*, const V&) {}

  void serialize(const char*, const char*, size_t) {}
  void serialize(const char *id, const object_base_ptr &x);

private:
  prototype_node &node_;
};

/// @endcond

}

#endif /* FOREIGN_KEY_ANALYZER_HPP */
