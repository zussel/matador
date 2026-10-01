#ifndef SINGLETON_HPP
#define SINGLETON_HPP

namespace matador::utils {

/**
 * @class singleton
 * @brief Provides only one instance of a type.
 * @tparam T The type of the singleton.
 * 
 * This class implements the singleton pattern.
 * It ensures that only one instance of a class
 * exists.
 * @note The derived class must add a friend declaration
 * for the concrete singleton type.
 */
template < typename T >
class singleton
{
public:
  typedef T value_type; /**< Shortcut for the singleton type */

  /**
   * @brief Access the instance of the class.
   * 
   * The static instance method provides
   * the access to the one instance of the
   * concrete class.
   * 
   * @return The one instance of the class.
   */
  static value_type& instance ()
  {
    static value_type instance_;
    return instance_;
  }
  virtual ~singleton() = default;

protected:
  singleton() = default;
};

}

#endif /* SINGLETON_HPP */
