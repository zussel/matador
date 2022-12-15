#ifndef OOS_DELETE_ACTION_HPP
#define OOS_DELETE_ACTION_HPP

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

#include "matador/object/action.hpp"

#include <string>

namespace matador {

class object_proxy;
class basic_identifier;

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
  delete_action(const char* classname, unsigned long id, object_proxy *proxy);

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
  basic_identifier* pk() const;

  unsigned long id() const;

  object_proxy* proxy() const;

  void backup(byte_buffer &buffer) override;

  void restore(byte_buffer &buffer, object_store *store) override;

  void mark_deleted();

private:
  std::string classname_;
  unsigned long id_ = 0;
  object_proxy *proxy_ = nullptr;

  bool deleted_ = false;
};

/// @endcond

}

#endif //OOS_DELETE_ACTION_HPP
