#ifndef OBJECT_PRODUCER_HPP
#define OBJECT_PRODUCER_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include <typeinfo>
#include <string>

namespace oos {

class serializable;

/**
* @class object_base_producer
* @brief Base class for serializable producer classes
*
* When using the object_store to create serializable
* an instance of a class of this type is used to
* create the serializable.
* The interface provides a create and a classname
* method.
*/
class OOS_API object_base_producer{
  public:
  virtual ~object_base_producer() {}

  /**
   * @brief Create a new serializable.
   *
   * This method creates a new serializable
   * and returns it.
   *
   * @return The created serializable.
   */
  virtual void* create() const = 0;

  /**
   * Returns the unique classname of the
   * serializable prototype.
   *
   * @return The classname of the serializable.
   */
  virtual const char *classname() const = 0;
};

/**
* @class object_producer
* @brief Produces a new serializable of type T
*
* These producers a placed in the serializable type tree and whenever
* a new serializable of a certain type is requested, the appropiate
* producer is used to create the serializable.
* It supports also a method to determine the name of the class
* which is produced.
*/
template<class T>
class object_producer : public object_base_producer {
public:
  virtual ~object_producer() {}

  virtual object_base_producer* clone() const
  {
    return new object_producer<T>();
  }

  /**
  * Creates and returns a new serializable of type T
  *
  * @return new serializable of type T
  */
  virtual void* create() const
  {
    return new T;
  }

  /**
  * Returns the name of the class which is created
  *
  * @return the name of the produced class
  */
  virtual const char* classname() const
  {
    return classname_.c_str();
  }

private:
  static std::string classname_;
};

template < class T >
std::string object_producer<T>::classname_ = typeid(T).name();

}

#endif /* OBJECT_PRODUCER_HPP */
