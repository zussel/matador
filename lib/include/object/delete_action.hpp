//
// Created by sascha on 12.03.16.
//

#ifndef OOS_DELETE_ACTION_HPP
#define OOS_DELETE_ACTION_HPP

#include "object/action.hpp"

#include "tools/basic_identifier.hpp"

namespace oos {

/**
 * @internal
 * @class delete_action
 * @brief Action when deleting an serializable.
 *
 * This action is used when an objected
 * is deleted from the database.
 */
class OOS_API delete_action : public action
{
public:
/**
 * Creates an delete_action.
 *
 * @param classname The serializable type name.
 * @param id The id of the deleted serializable.
 */
  delete_action(const char *classname, unsigned long id, basic_identifier *pk);

  virtual ~delete_action();

  virtual void accept(action_visitor *av);

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
  basic_identifier * pk() const;

  unsigned long id() const;

private:
  std::string classname_;
  unsigned long id_;
  std::unique_ptr<basic_identifier> pk_;
};

}

#endif //OOS_DELETE_ACTION_HPP
