#ifndef FOREIGN_KEY_ANALYZER_HPP
#define FOREIGN_KEY_ANALYZER_HPP

#include "object/access.hpp"
#include "object/object_ptr.hpp"

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
  void serialize(const V &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, const V&) {}

  void serialize(const char*, const char*, size_t) {}

  template < class T, bool TYPE >
  void serialize(const char*, const object_holder<T, TYPE> &)
  {
//    prototype_iterator node = node_.tree()->find(x.type());
//  if (node->producer == nullptr) {
//      // node isn't completely initialized yet
//      node->foreign_key_ids.push_back(std::make_pair(&node_, id));
//  } else if (node == node_.tree->end()) {
//    throw_object_exception("couldn't find prototype node of type '" << x.type() << "'");
//  } else if (!node->has_primary_key()) {
//    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
//  } else {
//    // node is inserted/attached; store it in nodes foreign key map
//    std::shared_ptr<basic_identifier> fk(node->id_->clone());
//    node_.foreign_keys.insert(std::make_pair(id, fk));
//  }
  }

private:
  prototype_node &node_;
};

/// @endcond

}

#endif /* FOREIGN_KEY_ANALYZER_HPP */
