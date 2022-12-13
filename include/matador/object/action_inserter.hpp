//
// Created by sascha on 11.03.16.
//

#ifndef OOS_ACTION_INSERTER_HPP
#define OOS_ACTION_INSERTER_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
#define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/object/action_visitor.hpp"
#include "matador/object/action.hpp"
#include "matador/object/insert_action.hpp"

#include <vector>

namespace matador {

/// @cond MATADOR_DEV

class object_proxy;

class MATADOR_OBJECT_API action_inserter : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;
  typedef t_action_vactor::iterator action_iterator;

public:
  action_inserter(t_action_vactor &actions);

  virtual ~action_inserter() { }

  template < class T >
  t_action_vactor::size_type insert(object_proxy *proxy);

  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);

private:
  void* object(object_proxy *proxy) const;
  const char* type(object_proxy *proxy) const;

private:
  std::reference_wrapper<t_action_vactor> actions_;
  object_proxy *proxy_ = nullptr;
  bool inserted_ = false;
};

template < class T >
action_inserter::t_action_vactor::size_type action_inserter::insert(object_proxy *proxy) {
  proxy_ = proxy;
  inserted_ = false;
  t_action_vactor::size_type end = actions_.get().size();
  for (t_action_vactor::size_type i = 0; i < end; ++i) {
//  while (first != last) {
    actions_.get().at(i)->accept(this);

//    (*first)->accept(this);
    if (inserted_) {
      return i;
    }
  }
  if (!inserted_) {
    auto ia = std::make_shared<insert_action>(type(proxy));
    ia->push_back(proxy_);
    actions_.get().push_back(ia);
    return actions_.get().size() - 1;
  }
  return actions_.get().size();
}

/// @endcond

}

#endif //OOS_ACTION_INSERTER_HPP
