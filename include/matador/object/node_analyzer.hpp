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

template < class T, template < class U = T > class O >
class node_analyzer
{
public:
  explicit node_analyzer(prototype_node &node, object_store &store, std::vector<O<T>*> observer = {})
    : node_(node), store_(store), observer_vector_(observer)
  { }

  ~node_analyzer() { }

  void analyze();
  void analyze(T &obj);

  template<class V>
  void serialize(V &x);
  template<class V>
  void serialize(const char *, V &) { }
  void serialize(const char *, char *, size_t) { }
  template<class V>
  void serialize(const char *id, belongs_to<V> &x, cascade_type);
  template<class V>
  void serialize(const char *id, has_one<V> &x, cascade_type);
  template<class V, template<class ...> class C>
  void serialize(const char *, has_many<V, C> &, const char *, const char *,
                 typename std::enable_if<!is_builtin<V>::value>::type* = 0);

  template<class V, template<class ...> class C>
  void serialize(const char *, has_many<V, C> &, const char *, const char *,
                 typename std::enable_if<is_builtin<V>::value>::type* = 0);

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
  std::vector<O<T>*> observer_vector_;
};

/// @endcond

}
}
#endif //OOS_NODE_ANALYZER_HPP
