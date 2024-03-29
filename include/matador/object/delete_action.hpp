#ifndef OOS_DELETE_ACTION_HPP
#define OOS_DELETE_ACTION_HPP

#include "matador/object/export.hpp"

#include "matador/object/action.hpp"

#include <string>

namespace matador {

class object_proxy;
class identifier;

/// @cond MATADOR_DEV

/**
 * @internal
 * @class delete_action
 * @brief Action when deleting an serializable.
 *
 * This action is used when an objected
 * is deleted from the database.
 */
class MATADOR_OBJECT_API delete_action : public action
{
public:
  /**
   * Creates an delete_action.
   *
   * @param classname The serializable type name.
   * @param id The id of the deleted serializable.
   */
  delete_action(const char* classname, unsigned long long id, object_proxy *proxy);

  /**
   * Creates an delete_action.
   *
   * @param classname The serializable type name.
   * @param id The id of the deleted serializable.
   */
  explicit delete_action(object_proxy *proxy);

  ~delete_action() override;

  void accept(action_visitor *av) override;

  /**
   * Return the class name of the
   * serializable.
   *
   * @return The class name.
   */
  const char* classname() const;

  /**
   * The primary key of the serializable of the action.
   *
   * @return The primary key of the deleted serializable.
   */
  const identifier& pk() const;

  unsigned long long id() const;

  object_proxy* proxy() const;

  void backup(byte_buffer &buffer) override;

  void restore(byte_buffer &buffer, object_store *store) override;

  void mark_deleted();

private:
  std::string classname_;
  unsigned long long id_ = 0;
  object_proxy *proxy_ = nullptr;

  bool deleted_ = false;
};

/// @endcond

}

#endif //OOS_DELETE_ACTION_HPP
