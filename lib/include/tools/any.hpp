#ifndef OOS_ANY_HPP
#define OOS_ANY_HPP

#include <cstddef>
#include <type_traits>
#include <tuple>
#include <typeindex>
#include <stdexcept>

namespace oos {

/**
 * @brief Simple any class
 *
 * Simple any class taken from
 * [here](http://codereview.stackexchange.com/a/48250).
 * This class is a template-less holder for
 * any type of object.
 */
class any
{
  using id = size_t;

  template<typename T>
  struct type { static void id() { } };

  template<typename T>
  static id type_id() { return reinterpret_cast<id>(&type<T>::id); }

  template<typename T>
  using decay = typename std::decay<T>::type;

  template<typename T>
  using none = typename std::enable_if<!std::is_same<any, T>::value>::type;

  struct base
  {
    virtual ~base() { }
    virtual bool is(id) const = 0;
    virtual base *copy() const = 0;
    virtual std::type_index type_index() const = 0;

  } *p = nullptr;

  template<typename T>
  struct data : base, std::tuple<T>
  {
    using std::tuple<T>::tuple;

    T       &get()      & { return std::get<0>(*this); }
    T const &get() const& { return std::get<0>(*this); }

    bool is(id i) const override { return i == type_id<T>(); }
    base *copy()  const override { return new data{get()}; }
    std::type_index type_index() const override { return std::type_index(typeid(T)); };
  };

  template<typename T>
  T &stat() { return static_cast<data<T>&>(*p).get(); }

  template<typename T>
  T const &stat() const { return static_cast<data<T> const&>(*p).get(); }

  template<typename T>
  T &dyn() { return dynamic_cast<data<T>&>(*p).get(); }

  template<typename T>
  T const &dyn() const { return dynamic_cast<data<T> const&>(*p).get(); }

public:
  /**
   * @brief Default constructor
   *
   * Create an empty anonymous any
   */
  any() { }
  ~any() { delete p; }

  /**
   * @brief Move copy constructor
   * @param s any object to be moved
   */
  any(any &&s)      : p{s.p} { s.p = nullptr; }
  /**
   * @brief Copy constructor
   * @param s any object to be copied
   */
  any(any const &s) : p{s.p->copy()} { }

  /**
   * @brief Create any from concrete object
   * @tparam T Type of object
   * @tparam U Decay type of object
   * @param x Concrete object
   */
  template<typename T, typename U = decay<T>, typename = none<U>>
  any(T &&x) : p{new data<U>{std::forward<T>(x)}} { }

  /**
   * @brief Assignment operator
   * @param s any object to be assigned
   * @return Reference to assigned any
   */
  any &operator=(any s) { swap(*this, s); return *this; }

  /**
   * @brief Get the type index of an any object.
   *
   * Get the type index of an any object. If any
   * doesn't hold a value a logic_error is thrown.
   *
   * @throws std::logic_error
   * @return The type_index of the underlying type
   */
  std::type_index type_index() const {
    if (!p) {
      throw std::logic_error("no type for any object");
    }
    return p->type_index();
  }
  /**
   * @brief Allow swapping of any objects
   * @param s Right hand any to swap
   * @param r left hand any to swap
   */
  friend void swap(any &s, any &r) { std::swap(s.p, r.p); }

  /**
   * @brief Clear any object
   *
   * Deletes the internal object
   */
  void clear() { delete p; p = nullptr; }

  /**
   * @brief Returns true if no object is assigned to any
   * @return True if no object is assigned to any
   */
  bool empty() const { return p == nullptr; }

  /**
   * @brief Checks type of any with given type
   * @tparam T Type to check with
   * @return True if types are the same
   */
  template<typename T>
  bool is() const { return p ? p->is(type_id<T>()) : false; }

  /**
   * @brief Returns the moved any object as static cast
   * @tparam T Type of any
   * @return The moved any object
   */
  template<typename T> T      &&_()     && { return std::move(stat<T>()); }

  /**
   * @brief Returns reference to any object as static cast
   * @tparam T Type of any
   * @return Reference to any object
   */
  template<typename T> T       &_()      & { return stat<T>(); }

  /**
   * @brief Returns const reference to any object as static cast
   * @tparam T Type of any
   * @return Const reference to any object
   */
  template<typename T> T const &_() const& { return stat<T>(); }

  /**
   * @brief Returns the moved any object as dynamic cast
   * @tparam T Type of any
   * @return The moved any object
   */
  template<typename T> T      &&cast()     && { return std::move(dyn<T>()); }

  /**
   * @brief Returns reference to any object as dynamic cast
   * @tparam T Type of any
   * @return Reference to any object
   */
  template<typename T> T       &cast()      & { return dyn<T>(); }

  /**
   * @brief Returns const reference to any object as dynamic cast
   * @tparam T Type of any
   * @return Const reference to any object
   */
  template<typename T> T const &cast() const& { return dyn<T>(); }

  /**
   * @brief Returns the moved any object as static cast
   * @tparam T Type of any
   * @return The moved any object
   */
  template<typename T> operator T     &&()     && { return std::move(_<T>()); }

  /**
   * @brief Returns reference to any object as static cast
   * @tparam T Type of any
   * @return Reference to any object
   */
  template<typename T> operator T      &()      & { return _<T>(); }

  /**
   * @brief Returns const reference to any object as static cast
   * @tparam T Type of any
   * @return Const reference to any object
   */
  template<typename T> operator T const&() const& { return _<T>(); }
};

}
#endif //OOS_ANY_HPP
