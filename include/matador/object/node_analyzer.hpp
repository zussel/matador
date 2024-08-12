#ifndef OOS_NODE_ANALYZER_HPP
#define OOS_NODE_ANALYZER_HPP

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/is_builtin.hpp"
#include "matador/utils/foreign_attributes.hpp"

namespace matador {

class prototype_node;

namespace detail {

/// @cond MATADOR_DEV

template < class Owner, template <typename> typename... ObserverType >
class node_analyzer
{
public:
  node_analyzer(prototype_node &node, object_store &store, const std::vector<std::unique_ptr<typed_object_store_observer<Owner>>> &observers)
    : node_(node), store_(store), observers_(observers)
  { }

  ~node_analyzer() = default;

  void analyze()
  {
    Owner obj;
    analyze(obj);
  }

  void analyze(Owner &obj)
  {
    matador::access::process(*this, obj);
  }

  template<class V>
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class V>
  void on_attribute(const char *, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }
  void on_attribute(const char *, char *, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }
  void on_attribute(const char *, std::string &, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }

  template<class Value>
  void on_belongs_to(const char *id, object_ptr<Value> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes);
  template<class Value>
  void on_has_one(const char *id, object_ptr<Value> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes);
  template<class Value, template<class ...> class Container>
  void on_has_many(const char *id, container<Value, Container> &x, const char *join_column, const utils::foreign_attributes &attr = utils::default_foreign_attributes, typename std::enable_if<!is_builtin<Value>::value>::type* = 0);
  template<class Value, template<class ...> class Container>
  void on_has_many(const char *id, container<Value, Container> &x, const char *join_column, const utils::foreign_attributes &attr = utils::default_foreign_attributes, typename std::enable_if<is_builtin<Value>::value>::type* = 0);
  template<class Value, template<class ...> class Container>
  void on_has_many(const char *id, container<Value, Container> &x, const utils::foreign_attributes &attr = utils::default_foreign_attributes, typename std::enable_if<!is_builtin<Value>::value>::type* = 0);
  template<class Value, template<class ...> class Container>
  void on_has_many(const char *id, container<Value, Container> &x, const utils::foreign_attributes &attr = utils::default_foreign_attributes, typename std::enable_if<is_builtin<Value>::value>::type* = 0);
  template<class Value, template<class ...> class Container>
  void on_has_many_to_many(const char *id, container<Value, Container> &x, const utils::foreign_attributes &attr = utils::default_foreign_attributes);
  template<class Value, template<class ...> class Container>
  void on_has_many_to_many(const char *, container<Value, Container> &, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes);

protected:
  prototype_node &node_;
  object_store &store_;

  const std::vector<std::unique_ptr<typed_object_store_observer<Owner>>> &observers_;
};

/// @endcond

}
}
#endif //OOS_NODE_ANALYZER_HPP
