#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/basic_has_many.hpp"

namespace oos {

class prototype_node;

namespace detail {

template < class T,  template < class ... > class ON_ATTACH >
class node_analyzer {
public:
  node_analyzer(prototype_node &node, const ON_ATTACH<T> &on_attach)
    : node_(node)
    , on_attach_(on_attach)
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
  prototype_node &node_;
  ON_ATTACH<T> on_attach_;
};

struct basic_on_attach {};

template < class T >
struct null_on_attach : public basic_on_attach
{
  null_on_attach() {}
  template < class V >
  null_on_attach(const null_on_attach<V> &) {}
  null_on_attach& operator=(const null_on_attach &) { return *this; }
  template < class V >
  null_on_attach& operator=(const null_on_attach<V> &) { return *this; }

  void operator()(prototype_node*) const {}
};

}
}
#endif //OOS_NODE_ANALYZER_HPP
