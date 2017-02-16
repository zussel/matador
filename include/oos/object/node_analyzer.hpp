#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/basic_has_many.hpp"

namespace oos {

class prototype_node;

namespace detail {

/// @cond OOS_DEV

class basic_node_analyzer
{
public:
  explicit basic_node_analyzer(prototype_node &node, object_store &store)
    : node_(node), store_(store)
  { }

protected:
  template < class V, class T >
  void process_belongs_to(const char *id, belongs_to <V> &x);

  template < class V, class T >
  void process_has_one(const char *id, has_one <V> &x);

  template < class V, class T, template<class ...> class C >
  void process_has_many(const prototype_iterator &pi, const char *id, has_many<V, C> &x);

  template < class V, class T >
  void register_has_many(const std::type_index &typeindex, const char *id, prototype_node *node);

protected:
  prototype_node &node_;
  object_store &store_;
};

template < class T > struct observer_dummy {};

template < class T, template < class U = T > class O = observer_dummy >
class node_analyzer : public basic_node_analyzer
{
public:
  explicit node_analyzer(prototype_node &node, object_store &store, const std::vector<O<T>*> &observer)
  : basic_node_analyzer(node, store), observer_(observer)
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
  const std::vector<O<T>*> &observer_;
};

template < class T >
class node_analyzer<T> : public basic_node_analyzer
{
public:
  explicit node_analyzer(prototype_node &node, object_store &store)
    : basic_node_analyzer(node, store)
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

};

/// @endcond

}
}
#endif //OOS_NODE_ANALYZER_HPP
