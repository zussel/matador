#ifndef OOS_OBJECT_DELETER_HPP
#define OOS_OBJECT_DELETER_HPP

#include "matador/object/export.hpp"

#include "matador/object/prototype_node.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/container.hpp"

#include "matador/utils/field_attributes.hpp"

#include <unordered_map>
#include <stack>

namespace matador {

class object_proxy;

namespace detail {

/**
 * @cond MATADOR_DEV
 * @class object_deleter
 * @brief Checks if an serializable could be deleted
 *
 * This class checks whether a given serializable or a
 * given object_list_base and their children objects
 * could be deleted or not.
 * If the check was successful, all the deletable serializable
 * can be accepted via the iterators.
 */
class MATADOR_OBJECT_API object_deleter {
private:
  struct MATADOR_OBJECT_API t_object_count {
    explicit t_object_count(object_proxy *oproxy);

    void remove();

    object_proxy *proxy;
  };

private:
  typedef std::map<unsigned long long, t_object_count> t_objects_to_remove_map;
  typedef std::vector<std::function<void()>> t_relations_to_remove_map;

public:
  typedef t_objects_to_remove_map::iterator iterator;
  /**< Shortcut the serializable map iterator */
  typedef t_objects_to_remove_map::const_iterator const_iterator; /**< Shortcut the serializable map const_iterator */

  /**
   * Creates an instance of the object_deleter
   */
  object_deleter() = default;

  ~object_deleter() = default;

  /**
   * Checks weather the given serializable is deletable.
   *
   * @param proxy The object_proxy to be checked.
   * @return True if the serializable could be deleted.
   */
  bool is_deletable(object_proxy *proxy);

  void remove();

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the first deletable serializable.
   */
  iterator begin();

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the last deletable serializable.
   */
  iterator end();

  template<class T>
  void serialize(T &x) { matador::access::process(*this, x); }

  template<class T>
  void on_primary_key(const char *id, T &x, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type* = 0);
  void on_primary_key(const char *id, std::string &x, size_t size);
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class T>
  void on_attribute(const char *, const T &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  template<class T>
  void on_belongs_to(const char *, object_ptr<T> &x, cascade_type cascade);
  template<class T>
  void on_has_one(const char *, object_ptr<T> &x, cascade_type cascade);
  template<class T, template<class ...> class Container>
  void on_has_many(const char *id, container<T, Container> &x, const char *, const char *, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }
  template<class T, template<class ...> class Container>
  void on_has_many(const char *, container<T, Container> &, cascade_type, typename std::enable_if<!is_builtin<T>::value>::type* = 0);
  template<class T, template<class ...> class Container>
  void on_has_many(const char *, container<T, Container> &, cascade_type, typename std::enable_if<is_builtin<T>::value>::type* = 0);

private:
  bool check_object_count_map() const;

private:
  t_objects_to_remove_map objects_to_remove_;
  t_relations_to_remove_map relations_to_remove_;
  std::unordered_map<object_proxy*, long> visited_objects_;
  std::stack<object_proxy*> proxy_stack_;
};

template<class T>
void object_deleter::on_belongs_to(const char *, object_ptr<T> &x, cascade_type cascade)
{
  if (!x.ptr()) {
    return;
  }

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

  if (!proxy_stack_.top()->node()->is_relation_node()) {
    if (x.proxy_ && x.relation_info_) {
      relations_to_remove_.push_back([&x]() {
        x.relation_info_->remove_value_from_foreign(x.owner_, x.proxy_);
      });
    }
    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1));
  }


  auto curr_obj = visited_objects_.find(proxy_stack_.top());
  if (curr_obj != visited_objects_.end()) {
    --curr_obj->second;
  }

  if ((cascade & cascade_type::REMOVE) == cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_)));
    proxy_stack_.push(x.proxy_);
    matador::access::process(*this, *(T*)x.ptr());
    proxy_stack_.pop();
  }
}

template<class T>
void object_deleter::on_has_one(const char *, object_ptr<T> &x, cascade_type cascade)
{
  if (!x.ptr()) {
    return;
  }

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

  if (!proxy_stack_.top()->node()->is_relation_node()) {
    if (x.proxy_ && x.relation_info_) {
      relations_to_remove_.push_back([&x]() {
        x.relation_info_->remove_value_from_foreign(x.owner_, x.proxy_);
      });
    }
    auto foreign_endpoint = x.relation_info_->foreign_endpoint.lock();
    if (foreign_endpoint && foreign_endpoint->type == basic_relation_endpoint::BELONGS_TO) {
      auto current_proxy = visited_objects_.find(proxy_stack_.top());
      if (current_proxy != visited_objects_.end()) {
        --current_proxy->second;
      }
    }
    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1));
  }

  if ((cascade & cascade_type::REMOVE) == cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_)));
    proxy_stack_.push(x.proxy_);
    matador::access::process(*this, *(T*)x.ptr());
    proxy_stack_.pop();
  }
}

template<class T, template<class ...> class C>
void object_deleter::on_has_many(const char *, container<T, C> &x, cascade_type, typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
  typename container<T, C>::iterator first = x.begin();
  typename container<T, C>::iterator last = x.end();

  object_proxy *owner = proxy_stack_.top();
  while (first != last) {
    container_item_holder<T> &holder = first.holder_item();
    if (x.relation_info_) {
      relations_to_remove_.push_back([&holder,&x, owner]() {
        x.relation_info_->remove_value_from_foreign(holder, owner);
      });
    }

    auto curr_obj = visited_objects_.find(proxy_stack_.top());
    if (curr_obj != visited_objects_.end()) {
      --curr_obj->second;
    }
    ++first;
  }
}

template<class T, template<class ...> class C>
void object_deleter::on_has_many(const char *, container<T, C> &x, cascade_type, typename std::enable_if<matador::is_builtin<T>::value>::type*)
{
  typename container<T, C>::iterator first = x.begin();
  typename container<T, C>::iterator last = x.end();

  while (first != last) {
    auto curr_obj = visited_objects_.find(proxy_stack_.top());
    if (curr_obj != visited_objects_.end()) {
      --curr_obj->second;
    }
    ++first;
  }
}

template<class T>
void object_deleter::on_primary_key(const char *id, T &x, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type*)
{
  on_attribute(id, x);
}
/// @endcond

}
}
#endif //OOS_OBJECT_DELETER_HPP
