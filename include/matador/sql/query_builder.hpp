#ifndef QUERY_QUERY_BUILDER_HPP
#define QUERY_QUERY_BUILDER_HPP

#include "matador/sql/any_type_to_string_visitor.hpp"
#include "matador/sql/basic_condition.hpp"
#include "matador/sql/column_definition.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect.hpp"
#include "matador/sql/key_value_pair.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/record.hpp"

#include "matador/utils/string.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace matador::sql {

namespace detail {
// struct any_type_to_string_visitor
// {
//   explicit any_type_to_string_visitor(const dialect &d, query_context &query);
//
//   void operator()(char &x) { to_string(x); }
//   void operator()(short &x) { to_string(x); }
//   void operator()(int &x) { to_string(x); }
//   void operator()(long &x) { to_string(x); }
//   void operator()(long long &x) { to_string(x); }
//   void operator()(unsigned char &x) { to_string(x); }
//   void operator()(unsigned short &x) { to_string(x); }
//   void operator()(unsigned int &x) { to_string(x); }
//   void operator()(unsigned long &x) { to_string(x); }
//   void operator()(unsigned long long &x) { to_string(x); }
//   void operator()(bool &x) { to_string(x); }
//   void operator()(float &x) { to_string(x); }
//   void operator()(double &x) { to_string(x); }
//   void operator()(const char *x) { to_string(x); }
//   void operator()(std::string &x) { to_string(x); }
//   void operator()(date &x) { utils::to_string(x); }
//   void operator()(time &x) { utils::to_string(x); }
//   void operator()(utils::blob &x) { to_string(x); }
//   void operator()(utils::placeholder &x) { to_string(x); }
//
//   template<typename Type>
//   void to_string(Type &val)
//   {
//     result = std::to_string(val);
//   }
//   void to_string(const char *val);
//   void to_string(std::string &val);
//   void to_string(utils::blob &val);
//   void to_string(utils::placeholder &val);
//
//   const dialect &d;
//   query_context &query;
//   std::string result;
// };

}

column alias(const std::string &column, const std::string &as);
column alias(column &&col, const std::string &as);
column count(const std::string &column);
column count_all();

enum class join_type_t {
  INNER, OUTER, LEFT, RIGHT
};

class query_builder
{
private:
  enum class state_t {
    QUERY_INIT,
    QUERY_CREATE,
    QUERY_TABLE_CREATE,
    QUERY_TABLE_DROP,
    QUERY_DROP,
    QUERY_SELECT,
    QUERY_INSERT,
    QUERY_UPDATE,
    QUERY_DELETE,
    QUERY_SET,
    QUERY_FROM,
    QUERY_JOIN,
    QUERY_ON,
    QUERY_INTO,
    QUERY_WHERE,
    QUERY_VALUES,
    QUERY_ORDER_BY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUP_BY,
    QUERY_OFFSET,
    QUERY_LIMIT,
    QUERY_FINISH
  };

  enum class command_t {
    UNKNOWN,   /**< Unknown query command */
    CREATE,    /**< Create query command */
    DROP,      /**< Drop query command */
    SELECT,    /**< Select query command */
    INSERT,    /**< Insert query command */
    UPDATE,    /**< Update query command */
    REMOVE     /**< Remove query command */
  };

  struct query_part
  {
    query_part(dialect_token t, std::string p)
    : token(t), part(std::move(p)) {}

    dialect_token token;
    std::string part;
  };

public:
  explicit query_builder(const dialect &d);

  query_builder& create();
  query_builder& drop();
  query_builder& select(std::initializer_list<column> columns);
  query_builder& select(const std::vector<column> &columns);
  query_builder& insert();
  query_builder& update(const std::string &table);
  query_builder& remove();

  query_builder& table(const std::string &table, std::initializer_list<column_definition> columns);
  query_builder& table(const std::string &table, const std::vector<column_definition> &columns);
  query_builder& table(const std::string &table);
  query_builder& into(const std::string &table, std::initializer_list<column> column_names);
  query_builder& into(const std::string &table, const std::vector<column> &column_names);
  query_builder& values(std::initializer_list<utils::any_type> values);
  query_builder& values(const std::vector<utils::any_type> &values);
  query_builder& from(const std::string &table, const std::string &as = "");
  query_builder& join(const std::string &table, join_type_t, const std::string &as = "");
  query_builder& on(const std::string &column, const std::string &join_column);
  query_builder& set(std::initializer_list<key_value_pair> key_values);
  query_builder& set(const std::vector<key_value_pair> &key_values);
  query_builder& where(const basic_condition &cond);
  query_builder& order_by(const std::string &column);
  query_builder& group_by(const std::string &column);
  query_builder& asc();
  query_builder& desc();
  query_builder& offset(size_t count);
  query_builder& limit(size_t count);

  query_context compile();

private:
  void transition_to(state_t next);
  void initialize(command_t cmd, state_t state);

private:
  const dialect &dialect_;

  command_t command_{command_t::UNKNOWN};
  state_t state_{state_t::QUERY_INIT};

  std::vector<query_part> query_parts_;

  any_type_to_string_visitor value_to_string_;

  query_context query_;

  using query_state_set = std::unordered_set<state_t>;
  using query_state_transition_map = std::unordered_map<state_t, query_state_set>;
  using query_state_to_string_map = std::unordered_map<state_t, std::string>;
  using query_command_to_string_map = std::unordered_map<command_t, std::string>;

  static query_state_transition_map transitions_;
  static query_state_to_string_map state_strings_;
  static query_command_to_string_map command_strings_;
};

}
#endif //QUERY_QUERY_BUILDER_HPP
