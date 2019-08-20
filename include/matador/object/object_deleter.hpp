#ifndef OOS_OBJECT_DELETER_HPP
#define OOS_OBJECT_DELETER_HPP

#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/basic_has_many.hpp"

#include <unordered_set>
#include <stack>

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define MATADOR_OBJECT_API
#endif

namespace matador {

class object_proxy;

namespace detail {

/**
 * @cond MATADOR_DEV
 * @class object_deleter
 * @brief Checks if an serializable could be deleted
 *
 * This class checks wether a given serializable or a
 * given object_list_base and their children objects
 * could be deleted or not.
 * If the check was successful, all the deletable serializable
 * can be accepted via the iterators.
 */
class MATADOR_OBJECT_API object_deleter {
private:
  struct MATADOR_OBJECT_API t_object_count {
    typedef void (*t_remove_func)(object_proxy*);
    template < class T >
    explicit t_object_count(object_proxy *oproxy, T* = nullptr)
      : proxy(oproxy)
      , remove_func(&remove_object<T>)
    {}

    void remove();

    template <typename T>
    static void remove_object(object_proxy *proxy);

    object_proxy *proxy;

    t_remove_func remove_func;
  };

private:
  typedef std::map<unsigned long, t_object_count> t_objects_to_remove_map;
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
   * Checks wether the given serializable is deletable.
   *
   * @param proxy The object_proxy to be checked.
   * @return True if the serializable could be deleted.
   */
  template<class T>
  bool is_deletable(object_proxy *proxy, const T *o);

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
  void serialize(T &x) { matador::access::serialize(*this, x); }

  template<class T>
  void serialize(const char *, const T &);
  void serialize(const char *, char *, size_t);

  template<class T>
  void serialize(const char *, belongs_to<T> &x, cascade_type cascade);
  template<class T>
  void serialize(const char *, has_one<T> &x, cascade_type cascade);
  template<class T, template<class ...> class Container>
  void serialize(const char *id, basic_has_many<T, Container> &x, const char *, const char *, cascade_type cascade)
  {
    serialize(id, x, cascade);
  }

  template<class T, template<class ...> class Container>
  void serialize(const char *, basic_has_many<T, Container> &, cascade_type, typename std::enable_if<!is_builtin<T>::value>::type* = 0);
  template<class T, template<class ...> class Container>
  void serialize(const char *, basic_has_many<T, Container> &, cascade_type, typename std::enable_if<is_builtin<T>::value>::type* = 0) {}
  template<class T>
  void serialize(const char *id, identifier<T> &x);

private:
  bool check_object_count_map() const;

private:
  t_objects_to_remove_map objects_to_remove_;
  t_relations_to_remove_map relations_to_remove_;
  std::unordered_map<object_proxy*, long> visited_objects_;
  std::stack<object_proxy*> proxy_stack_;
};

/// @endcond

}
}
#endif //OOS_OBJECT_DELETER_HPP
