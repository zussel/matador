#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/basic_has_many.hpp"

#include "matador/utils/is_builtin.hpp"

namespace matador {

class prototype_node;

namespace detail {

/// @cond MATADOR_DEV

template < class Owner, template < class U = Owner > class Observer >
class node_analyzer
{
public:
  explicit node_analyzer(prototype_node &node, object_store &store, std::vector<Observer<Owner>*> observer = {})
    : node_(node), store_(store), observer_vector_(observer)
  { }

  ~node_analyzer() { }

  void analyze();
  void analyze(Owner &obj);

  template<class V>
  void serialize(V &x);
  template<class V>
  void serialize(const char *, V &) { }
  void serialize(const char *, char *, size_t) { }
  template<class Value>
  void serialize(const char *id, belongs_to<Value> &x, cascade_type);
  template<class Value>
  void serialize(const char *id, has_one<Value> &x, cascade_type);
  template<class Value, template<class ...> class Container>
  void serialize(const char *, has_many<Value, Container> &, const char *, const char *,
                 typename std::enable_if<!is_builtin<Value>::value>::type* = 0);

  template<class Value, template<class ...> class Container>
  void serialize(const char *, has_many<Value, Container> &, const char *, const char *,
                 typename std::enable_if<is_builtin<Value>::value>::type* = 0);

private:
  /**
   * Detach has_one_to_many node
   *
   * @param node Node to detach
   * @return The foreign prototype node iterator
   */
  template < class V >
  prototype_iterator detach_one_to_many_node(prototype_iterator node);

protected:
  prototype_node &node_;
  object_store &store_;
  std::vector<Observer<Owner>*> observer_vector_;
};

/// @endcond

}
}
#endif //OOS_NODE_ANALYZER_HPP
