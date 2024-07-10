#ifndef SQL_HPP
#define SQL_HPP

#include "matador/sql/token.hpp"
#include "matador/sql/commands.hpp"
#include "matador/sql/token_list.hpp"

#include "matador/utils/data_types.hpp"

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

class sql_context
{
public:
  sql_context();
  ~sql_context();

  void append(const std::shared_ptr<detail::token> &token_ptr);
  void append(const sql_context &stmt);

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

  t_query_command command_type_;
  token_list_t token_list_;

  std::string table_name_;
};

namespace detail {

struct query : public token
{
  explicit query(const sql_context &s);

  void accept(token_visitor &visitor) override;

  sql_context sql_;
};

}
/// @endcond

}

#endif /* SQL_HPP */
