//
// Created by sascha on 3/9/16.
//

#ifndef OOS_TRANSACTION_HPP
#define OOS_TRANSACTION_HPP

#include <iostream>

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

namespace oos {

class object_store;

class OOS_API transaction
{
public:
  explicit transaction(object_store &store);

  void begin();
  void commit();
  void rollback();
  void abort();

  template < class T >
  void on_insert(object_proxy *proxy);
  template < class T >
  void on_update(object_proxy *proxy);
  template < class T >
  void on_delete(object_proxy *proxy);

private:
  object_store &store_;
};

template<class T>
void transaction::on_insert(object_proxy *proxy)
{
  // create insert action
  T *obj = proxy->obj<T>();
  std::cout << "obj: " << obj << '\n';
}

template<class T>
void transaction::on_update(object_proxy *proxy)
{
  // create update action
  T *obj = proxy->obj<T>();
  std::cout << "obj: " << obj << '\n';
}

template<class T>
void transaction::on_delete(object_proxy *proxy)
{
  // create delete action
  T *obj = proxy->obj<T>();
  std::cout << "obj: " << obj << '\n';
}

}
#endif //OOS_TRANSACTION_HPP
