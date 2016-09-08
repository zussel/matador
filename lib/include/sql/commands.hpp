/**
 * @author sascha on 5/10/16.
 */

#ifndef OOS_COMMANDS_HPP
#define OOS_COMMANDS_HPP

namespace oos {

/**
 * Enumeration of query command types
 */
enum struct t_query_command {
  UNKNOWN,   /**< Unknown query command */
  CREATE,    /**< Create query command */
  DROP,      /**< Drop query command */
  SELECT,    /**< Select query command */
  INSERT,    /**< Insert query command */
  UPDATE,    /**< Update query command */
  REMOVE     /**< Remove query command */
};

}

#endif //OOS_COMMANDS_HPP
