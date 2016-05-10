/**
 * @author sascha on 5/10/16.
 */

#ifndef OOS_COMMANDS_HPP
#define OOS_COMMANDS_HPP

namespace oos {

enum struct t_query_command {
  UNKNOWN,
  CREATE,
  DROP,
  SELECT,
  INSERT,
  UPDATE,
  DELETE
};

}

#endif //OOS_COMMANDS_HPP
