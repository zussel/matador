#include <utility>

//
// Created by sascha on 12.03.16.
//

#ifndef OOS_INSERT_ACTION_HPP
#define OOS_INSERT_ACTION_HPP

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

#include <algorithm>

#include "matador/object/action.hpp"

namespace matador {

class object_proxy;
class object_serializer;

/// @cond MATADOR_DEV

/**
 * @internal
 * @class insert_action
 * @brief Action when inserting an serializable.
 *
 * This action is used when an objected
 * is inserted into the database.
 */
class MATADOR_OBJECT_API insert_action : public action
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;
  typedef object_proxy_list_t::iterator iterator;
  typedef object_proxy_list_t::const_iterator const_iterator;

public:
  /**
   * Creates an insert_action.
   *
   * @param type The type of the expected objects
   */
  template < class T >
  explicit insert_action(std::string type, T*)
    : type_(std::move(type))
  {}

  void accept(action_visitor *av) override;

  /**
   * Return the object type
   * of the action.
   *
   * @return The object type of the action
   */
  std::string type() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool empty() const;

  iterator find(unsigned long id);
  const_iterator find(unsigned long id) const;

  void push_back(object_proxy *proxy);

  iterator erase(iterator i);

  void backup(byte_buffer &) override { }

  void restore(byte_buffer &, object_store *store) override;

private:
  std::string type_;
  object_proxy_list_t object_proxy_list_;
};

/// @endcond

}

#endif //OOS_INSERT_ACTION_HPP
