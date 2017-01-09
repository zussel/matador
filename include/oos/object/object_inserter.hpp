#ifndef OOS_OBJECT_INSERTER_HPP
#define OOS_OBJECT_INSERTER_HPP

#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/basic_has_many.hpp"

#include <stack>

#ifdef _MSC_VER
#ifdef oos_object_EXPORTS
    #define OOS_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define OOS_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_OBJECT_API
#endif

namespace oos {
namespace detail {

/**
 * @cond OOS_DEV
 * @class object_inserter
 * @brief Creates objects and object_lists
 *
 * When an serializable is inserted into the serializable store
 * subsequently other serializable must be created and
 * inserted into the serializable store.
 * This class does these tasks.
 */
class OOS_OBJECT_API object_inserter {
  public:
  /**
   * @brief Creates an object_inserter instance.
   *
   * An object_inserter instance ist created for a
   * given object_store. The notify flag tells the
   * object_inserter wether the observers should be
   * notified or not.
   *
   * @param ostore The object_store.
   */
  object_inserter(object_store &ostore);

  ~object_inserter();

  template<class T>
  void insert(object_proxy *proxy, T *o, bool notify);

  void reset();

  template<class T>
  void serialize(T &x);

  template<class T>
  void serialize(const char *, T &) { }
  void serialize(const char *, char *, size_t) { }

  template<class T>
  void serialize(const char *, belongs_to<T> &x, cascade_type cascade);
  template<class T>
  void serialize(const char *, has_one<T> &x, cascade_type cascade);

  template<class T, template<class ...> class C>
  void serialize(const char *id, basic_has_many<T, C> &x, const char *owner_field, const char *item_field);

  private:
  typedef std::set<object_proxy *> t_object_proxy_set;

  t_object_proxy_set object_proxies_;

  std::stack<object_proxy *> object_proxy_stack_;

  object_store &ostore_;

  std::function<void(object_store&, object_proxy*)> modified_marker_;

  bool notify_ = false;
};

}
}

#endif //OOS_OBJECT_INSERTER_HPP
