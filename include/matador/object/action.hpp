#ifndef ACTION_HPP
#define ACTION_HPP

#include "matador/object/export.hpp"

namespace matador {

class action_visitor;
class byte_buffer;
class object_store;
class object_serializer;
class object_deserializer;

/// @cond MATADOR_DEV

/**
 * @class action
 * @brief Base class for all sql relevant actions
 *
 * The action class is the base class for all sql
 * relevant actions like:
 *  - insert
 *  - update
 *  - delete
 *
 * All these actions are accepted via the action_visitor
 * class, which implements the visitor pattern.
 */
class MATADOR_OBJECT_API action
{
public:
  action();
  virtual ~action();
  
  /**
   * @brief Interface to accept the action.
   * 
   * @param av The action_visitor
   */
  virtual void accept(action_visitor *av) = 0;

  /**
   * @brief Backup the object to the given byte_buffer
   *
   * @param to The byte_buffer to backup to
   */
  virtual void backup(byte_buffer &to) = 0;

  /**
   * @brief Restores an object from given byte_buffer into given object_store
   *
   * @param from byte_buffer to restore_ from
   * @param store object_store to insert in
   */
  virtual void restore(byte_buffer &from, object_store *store) = 0;

protected:
  object_serializer *serializer_;
  object_deserializer *deserializer_;

};

/// @endcond

}

#endif /* ACTION_HPP */
