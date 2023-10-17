#ifndef SQL_HPP
#define SQL_HPP

#include "matador/sql/types.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/commands.hpp"
#include "matador/sql/token_list.hpp"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>
#include <memory>

namespace matador {

/// @cond MATADOR_DEV

namespace detail {

class basic_dialect_compiler;
class basic_dialect_linker;
struct build_info;

}

class sql
{
public:
  sql() = default;
  ~sql() = default;

  void append(const std::shared_ptr<detail::token> &token_ptr);
  void append(const sql &stmt);

  void reset(t_query_command command_type);

  template < class T >
  static unsigned int type()
  {
    return data_type_traits<T>::type();
  }

  std::string command() const;
  std::string table_name() const;
  void table_name(const std::string &tname);

private:
  friend class basic_dialect;
  friend struct detail::build_info;
  friend class detail::basic_dialect_compiler;
  friend class detail::basic_dialect_linker;
  template < class L, class R, class E >
  friend class condition;

  t_query_command command_type_{t_query_command::UNKNOWN};
  token_list_t token_list_;

  std::string table_name_;
};

namespace detail {

struct query : public token
{
  explicit query(const sql &s);

  void accept(token_visitor &visitor) override;

  sql sql_;
};

}
/// @endcond

}

#endif /* SQL_HPP */
