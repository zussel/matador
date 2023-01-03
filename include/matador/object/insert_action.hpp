#ifndef OOS_INSERT_ACTION_HPP
#define OOS_INSERT_ACTION_HPP

#include "matador/object/export.hpp"

#include "matador/object/action.hpp"

#include <list>
#include <string>

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
  explicit insert_action(std::string type);

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
