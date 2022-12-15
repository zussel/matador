#ifndef OOS_UPDATE_ACTION_HPP
#define OOS_UPDATE_ACTION_HPP

#include "matador/object/action.hpp"
#include "matador/object/delete_action.hpp"

#include <memory>

namespace matador {

class object_proxy;

/// @cond MATADOR_DEV

/**
 * @internal
 * @class update_action
 * @brief Action when updating an serializable.
 *
 * This action is used when an objected
 * is updated on the database.
 */
class MATADOR_OBJECT_API update_action : public action
{
public:
  /**
   * Creates an update_action.
   *
   * @param o The updated serializable.
   */
  explicit update_action(object_proxy *proxy);

  void accept(action_visitor *av) override;

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const;

  void backup(byte_buffer &buffer) override;

  void restore(byte_buffer &buffer, object_store *store) override;

  delete_action* release_delete_action();

private:
  object_proxy *proxy_;
  std::unique_ptr<delete_action> delete_action_;
};

/// @endcond

}

#endif //OOS_UPDATE_ACTION_HPP
