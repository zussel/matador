#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/basic_has_many.hpp"

namespace oos {

class prototype_node;

namespace detail {

template < class T, template < class V = T > class ... O >
class node_analyzer {
public:
  explicit node_analyzer(prototype_node &node, O<T>*... observer)
    : node_(node), observer_(observer...)
  { }

  ~node_analyzer() { }

  void analyze();

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
                 typename std::enable_if<!std::is_scalar<V>::value>::type* = 0);

  template<class V, template<class ...> class C>
  void serialize(const char *, has_many<V, C> &, const char *, const char *,
                 typename std::enable_if<std::is_scalar<V>::value>::type* = 0);

private:
  template < class IT, std::size_t... Is >
  prototype_iterator attach(object_store &store, const char *id, const std::tuple<O<T>*...>& tuple, std::index_sequence<Is...>);

private:
  prototype_node &node_;
  std::tuple<O<T>*...> observer_;
};

}
}
#endif //OOS_NODE_ANALYZER_HPP
