//
// Created by sascha on 11.03.16.
//

#ifndef OOS_ACTION_VISITOR_HPP
#define OOS_ACTION_VISITOR_HPP

#ifdef _MSC_VER
#ifdef oos_object_EXPORTS
    #define OOS_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define OOS_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_OBJECT_API
#endif

namespace oos {

/// @cond OOS_DEV

class insert_action;
class update_action;
class delete_action;

/**
 * @class action_visitor
 * @brief The action_visitor class accepts all action types.
 *
 * The action_visitor class accepts all concrete action
 * type classes. It implements the visitor pattern. To have
 * a concrete visitor, a class must be derived from this
 * class and implement all virtual method.
 */
class OOS_OBJECT_API action_visitor
{
public:
  virtual ~action_visitor() {}

  /**
   * @brief Called with a insert_action
   *
   * When a new serializable inserted into the
   * object_store a new insert_action with
   * newly created serializable is created.
   *
   * @param a The insert_action to visit.
   */
  virtual void visit(insert_action *a) = 0;

  /**
   * @brief Called with a update_action
   *
   * When an serializable updated in the
   * object_store a new update_action
   * with modified serializable is created.
   *
   * @param a The update_action to visit.
   */
  virtual void visit(update_action *a) = 0;

  /**
   * @brief Called with a delete_action
   *
   * When an serializable deleted from the
   * object_store a new delete_action
   * with deleted serializable is created.
   *
   * @param a The delete_action to visit.
   */
  virtual void visit(delete_action *a) = 0;
};

/// @endcond

}
#endif //OOS_ACTION_VISITOR_HPP
