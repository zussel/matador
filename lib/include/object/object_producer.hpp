#ifndef OBJECT_PRODUCER_HPP
#define OBJECT_PRODUCER_HPP

#ifdef WIN32
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

namespace oos {
/**
* @class object_base_producer
* @brief Base class for object producer classes
*
* When using the object_store to create object
* an instance of a class of this type is used to
* create the object.
* The interface provides a create and a classname
* method.
*/
class OOS_API object_base_producer{
  public:
  virtual ~object_base_producer() {
  }

  /**
  * @brief Create a new object.
  *
  * This method creates a new object
  * and returns it.
  *
  * @return The created object.
  */
  virtual object* create() const = 0;

  /**
  * Returns the unique classname of the
  * object prototype.
  *
  * @return The classname of the object.
  */
  virtual const char *classname() const = 0;
};

/**
* @class object_producer
* @brief Produces a new object of type T
*
* These producers a placed in the object type tree and whenever
* a new object of a certain type is requested, the appropiate
* producer is used to create the object.
* It supports also a method to determine the name of the class
* which is produced.
*/
template<class T>
class object_producer : public object_base_producer {
public:
  virtual ~object_producer() {
  }

  /**
  * Creates and returns a new object of type T
  *
  * @return new object of type T
  */
  virtual object *create() const {
    return new T;
  }

  /**
  * Returns the name of the class which is created
  *
  * @return the name of the produced class
  */
  virtual const char *classname() const {
    return typeid(T).name();
  }
};

}

#endif /* OBJECT_PRODUCER_HPP */
