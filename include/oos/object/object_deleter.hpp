#ifndef OOS_OBJECT_DELETER_HPP
#define OOS_OBJECT_DELETER_HPP

#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/basic_has_many.hpp"

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

class object_proxy;

namespace detail {

/**
 * @cond OOS_DEV
 * @class object_deleter
 * @brief Checks if an serializable could be deleted
 *
 * This class checks wether a given serializable or a
 * given object_list_base and their children objects
 * could be deleted or not.
 * If the check was successful, all the deletable serializable
 * can be accepted via the iterators.
 */
class OOS_OBJECT_API object_deleter {
  private:
  struct OOS_OBJECT_API t_object_count {
    typedef void (*t_remove_func)(object_proxy*, bool);
    template < class T >
    t_object_count(object_proxy *oproxy, bool ignr = true, T* = nullptr)
    : proxy(oproxy)
      , reference_counter(oproxy->reference_count())
      , ignore(ignr)
      , remove_func(&remove_object<T>)
    {}

    void remove(bool notify);

    template <typename T>
    static void remove_object(object_proxy *proxy, bool notify);

    object_proxy *proxy;
    unsigned long reference_counter;
    bool ignore;

    t_remove_func remove_func;
  };

  private:
  typedef std::map<unsigned long, t_object_count> t_object_count_map;

  public:
  typedef t_object_count_map::iterator iterator;
  /**< Shortcut the serializable map iterator */
  typedef t_object_count_map::const_iterator const_iterator; /**< Shortcut the serializable map const_iterator */

  /**
   * Creates an instance of the object_deleter
   */
  object_deleter() { }

  ~object_deleter() { }

  /**
   * Checks wether the given serializable is deletable.
   *
   * @param proxy The object_proxy to be checked.
   * @return True if the serializable could be deleted.
   */
  template<class T>
  bool is_deletable(object_proxy *proxy, T *o);

  /**
   * Checks wether the given object_container is deletable.
   *
   * @param ovector The object_container to be checked.
   * @return True if the object_container could be deleted.
   */
//  bool is_deletable(object_container &oc);

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
  void serialize(T &x) { oos::access::serialize(*this, x); }

  template<class T>
  void serialize(const char *, T &) { }
  void serialize(const char *, char *, size_t) { }

  template<class T>
  void serialize(const char *, belongs_to<T> &x, cascade_type cascade);
  template<class T>
  void serialize(const char *, has_one<T> &x, cascade_type cascade);
  template<class T, template<class ...> class C>
  void serialize(const char *, basic_has_many<T, C> &, const char *, const char *);
  template<class T>
  void serialize(const char *id, identifier<T> &x);

  bool check_object_count_map() const;

  private:
  t_object_count_map object_count_map;
};

/// @endcond

}
}
#endif //OOS_OBJECT_DELETER_HPP
