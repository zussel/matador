#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/is_builtin.hpp"

namespace matador {

class prototype_node;

namespace detail {

/// @cond MATADOR_DEV

template < class Owner, template < class U = Owner > class Observer >
class node_analyzer
{
public:
  node_analyzer(prototype_node &node, object_store &store, std::vector<Observer<Owner>*> observer = {})
    : node_(node), store_(store), observer_vector_(observer)
  { }

  ~node_analyzer() = default;

  void analyze();
  void analyze(Owner &obj);

  template<class V>
  void serialize(V &x);
  template<class V>
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class V>
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) { }
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) { }
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) { }
  template<class Value>
  void on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type);
  template<class Value>
  void on_has_one(const char *id, object_ptr<Value> &x, cascade_type);

  template<class Value, template<class ...> class Container>
  void on_has_many(const char *id, container<Value, Container> &x, cascade_type cascade);

  template<class Value, template<class ...> class Container>
  void on_has_many(const char *, container<Value, Container> &, const char *left_column, const char *right_column, cascade_type, typename std::enable_if<!is_builtin<Value>::value>::type* = 0);
  template<class Value, template<class ...> class Container>
  void on_has_many(const char *, container<Value, Container> &, const char *left_column, const char *right_column, cascade_type, typename std::enable_if<is_builtin<Value>::value>::type* = 0);

private:
  /**
   * Detach has_one_to_many node
   *
   * @param node Node to detach
   * @return The foreign prototype node iterator
   */
  template < class V >
  prototype_iterator detach_one_to_many_node(const prototype_iterator &node);

protected:
  prototype_node &node_;
  object_store &store_;
  std::vector<Observer<Owner>*> observer_vector_;
};

/// @endcond

}
}
#endif //OOS_NODE_ANALYZER_HPP
