#include <matador/sql/typed_column_serializer.hpp>
#include <matador/sql/column.hpp>
#include <matador/sql/columns.hpp>

#include <iostream>
#include <memory>
#include <utility>
#include <unordered_map>
#include <unordered_set>

template < class Type >
class result
{
public:
};

template < class Type >
class statement
{
public:
};

struct column{
  explicit column(std::string name) :name(std::move(name)) {}

  std::string name;
};

struct row
{

};
class connection;

enum class OrderDirection {
  ASC, DESC
};

enum class QueryState {
  QUERY_INIT,
  QUERY_CREATE,
  QUERY_DROP,
  QUERY_SELECT,
  QUERY_INSERT,
  QUERY_UPDATE,
  QUERY_DELETE,
  QUERY_FROM,
  QUERY_TABLE_CREATE,
  QUERY_TABLE_DROP,
  QUERY_SET,
  QUERY_INTO,
  QUERY_VALUES,
  QUERY_WHERE,
  QUERY_ORDER_BY,
  QUERY_GROUP_BY,
  QUERY_LIMIT,
  QUERY_FINISH,
};

class create_token;
class drop_token;
class table_token;

class statement_token_visitor
{
public:
  virtual void visit(create_token &ct) = 0;
  virtual void visit(drop_token &ct) = 0;
  virtual void visit(table_token &ct) = 0;
};

enum class StatementTokenType {
  TOKEN_CREATE,
  TOKEN_DROP,
  TOKEN_TABLE
};

class statement_token
{
public:
  virtual ~statement_token() = default;
  virtual void accept(statement_token_visitor &visitor) = 0;

protected:
  explicit statement_token(StatementTokenType type)
  : type(type) {}

private:
  StatementTokenType type;
};

class create_token : public statement_token
{
public:
  create_token() : statement_token(StatementTokenType::TOKEN_CREATE) {}

  void accept(statement_token_visitor &visitor) override {
    visitor.visit(*this);
  }
};

class drop_token : public statement_token
{
public:
  drop_token() : statement_token(StatementTokenType::TOKEN_DROP) {}

  void accept(statement_token_visitor &visitor) override {
    visitor.visit(*this);
  }
};

class table_token : public statement_token
{
public:
  table_token() : statement_token(StatementTokenType::TOKEN_TABLE) {}

  void accept(statement_token_visitor &visitor) override {
    visitor.visit(*this);
  }
};

class query
{
public:
  explicit query(connection &c)
  {}

  query& create() {
    transition_to(QueryState::QUERY_CREATE);
    return *this;
  };

  query& drop() {
    transition_to(QueryState::QUERY_DROP);
    return *this;
  };

  template<class Type>
  query& table(const std::string &table_name) {
    Type obj;
    return table(table_name, obj);
  }

  template<class Type>
  query& table(const std::string &table_name, const Type &obj) {
    return table(table_name, typed_column_serializer_.execute(const_cast<Type&>(obj)));
  }

  query& table(const std::string &table_name, const std::initializer_list<std::shared_ptr<matador::column>> &/*column_list*/) {
    transition_to(QueryState::QUERY_CREATE);
    return *this;
  }

  query& table(const std::string &table_name, const std::shared_ptr<matador::columns> &/*column_list*/) {
    transition_to(QueryState::QUERY_TABLE_DROP);
    return *this;
  }

  query& table(const std::string &table_name, const row &/*r*/) {
    transition_to(QueryState::QUERY_CREATE);
    return *this;
  }

  template<class Type>
  query& select()
  {
    transition_to(QueryState::QUERY_SELECT);
    const Type t;
    return select(t);
  }
  template<class Type>
  query& select(const Type &t) {
    transition_to(QueryState::QUERY_SELECT);
    return *this;
  }
  query& select(const std::initializer_list<std::shared_ptr<column>> &/*column_list*/) {
    transition_to(QueryState::QUERY_SELECT);
    return *this;
  }
  query& from(const std::string &table_name) {
    transition_to(QueryState::QUERY_FROM);
    return *this;
  }
  query& where() {
    transition_to(QueryState::QUERY_WHERE);
    return *this;
  }

  query& insert() {
    transition_to(QueryState::QUERY_INSERT);
    return *this;
  }
  query& into(const std::string &table_name) {
    transition_to(QueryState::QUERY_INTO);
    return *this;
  }
  query& values() {
    transition_to(QueryState::QUERY_VALUES);
    return *this;
  }

  query& update(const std::string &table_name) {
    transition_to(QueryState::QUERY_UPDATE);
    return *this;
  }
  query& set() {
    transition_to(QueryState::QUERY_SET);
    return *this;
  }

  query& remove() {
    transition_to(QueryState::QUERY_DELETE);
    return *this;
  }

  query& limit() {
    transition_to(QueryState::QUERY_LIMIT);
    return *this;
  }
  query& order_by(const std::string &column_name, OrderDirection direction = OrderDirection::ASC) {
    transition_to(QueryState::QUERY_ORDER_BY);
    return *this;
  }
  query& group_by(const std::initializer_list<std::shared_ptr<column>> &/*column_list*/) {
    transition_to(QueryState::QUERY_GROUP_BY);
    return *this;
  }

  template <class Type>
  result<Type> execute() {
    transition_to(QueryState::QUERY_FINISH);
    return {};
  }
  result<row> execute() {
    transition_to(QueryState::QUERY_FINISH);
    return {};
  }
  template <class Type>
  statement<Type> prepare() {
    transition_to(QueryState::QUERY_FINISH);
    return {};
  }
  statement<row> prepare() {
    transition_to(QueryState::QUERY_FINISH);
    return {};
  }

private:
  void transition_to(QueryState next)
  {
    if (transitions_[state_].count(next) == 0) {
      throw std::logic_error("invalid next state " + state_strings_[next]);
    }
    state_ = next;
  }
private:
  QueryState state_{QueryState::QUERY_INIT};

  matador::detail::typed_column_serializer typed_column_serializer_;

  using QueryStates = std::unordered_set<QueryState>;
  using TransitionMap = std::unordered_map<QueryState, QueryStates>;
  using QueryStateStringMap = std::unordered_map<QueryState, std::string>;

  static TransitionMap transitions_;
  static QueryStateStringMap state_strings_;
};

query::TransitionMap query::transitions_{
{QueryState::QUERY_INIT, {QueryState::QUERY_CREATE, QueryState::QUERY_DROP, QueryState::QUERY_SELECT, QueryState::QUERY_INSERT, QueryState::QUERY_UPDATE, QueryState::QUERY_DELETE}},
{QueryState::QUERY_CREATE, {QueryState::QUERY_TABLE_CREATE}},
{QueryState::QUERY_DROP, {QueryState::QUERY_TABLE_DROP}},
{QueryState::QUERY_SELECT, {QueryState::QUERY_FROM}},
{QueryState::QUERY_INSERT, {QueryState::QUERY_INTO}},
{QueryState::QUERY_UPDATE, {QueryState::QUERY_SET}},
{QueryState::QUERY_DELETE, {QueryState::QUERY_FROM}},
{QueryState::QUERY_TABLE_CREATE, {QueryState::QUERY_FINISH}},
{QueryState::QUERY_TABLE_DROP, {QueryState::QUERY_FINISH}},
{QueryState::QUERY_FROM, {QueryState::QUERY_WHERE, QueryState::QUERY_FINISH}},
{QueryState::QUERY_SET, {QueryState::QUERY_WHERE, QueryState::QUERY_FINISH}},
{QueryState::QUERY_INTO, {QueryState::QUERY_VALUES}},
{QueryState::QUERY_WHERE, {QueryState::QUERY_ORDER_BY, QueryState::QUERY_GROUP_BY, QueryState::QUERY_LIMIT, QueryState::QUERY_FINISH}},
{QueryState::QUERY_ORDER_BY, {QueryState::QUERY_LIMIT, QueryState::QUERY_FINISH}},
{QueryState::QUERY_GROUP_BY, {QueryState::QUERY_FINISH}},
{QueryState::QUERY_LIMIT, {QueryState::QUERY_FINISH}},
{QueryState::QUERY_VALUES, {QueryState::QUERY_FINISH}},
{QueryState::QUERY_FINISH, {}},
};

query::QueryStateStringMap query::state_strings_ {
{ QueryState::QUERY_INIT, "init" },
{ QueryState::QUERY_CREATE, "create" },
{ QueryState::QUERY_DROP, "drop" },
{ QueryState::QUERY_SELECT, "select" },
{ QueryState::QUERY_INSERT, "insert" },
{ QueryState::QUERY_UPDATE, "update" },
{ QueryState::QUERY_DELETE, "delete" },
{ QueryState::QUERY_TABLE_CREATE, "table" },
{ QueryState::QUERY_TABLE_DROP, "table" },
{ QueryState::QUERY_FROM, "from" },
{ QueryState::QUERY_SET, "set" },
{ QueryState::QUERY_INTO, "into" },
{ QueryState::QUERY_WHERE, "where" },
{ QueryState::QUERY_ORDER_BY, "order_by" },
{ QueryState::QUERY_GROUP_BY, "group_by" },
{ QueryState::QUERY_LIMIT, "limit" },
{ QueryState::QUERY_FINISH, "finish" },
};

class connection
{
public:
  query query()
  { return ::query{*this}; }
};

struct person{
  unsigned long id{};
  std::string name;

  template<typename Operator>
  void process(Operator &op) {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
  }
};

struct select_event{};

int main() {
  try {
    connection c;

    person p;
    auto &q0 = c.query().create().table<person>("person");
//    auto &q1 = c.query().create<person>().table("person");
//    auto &q2 = c.query().create({std::make_shared<matador::column>("id")}).table("person");
//    auto &q3 = c.query().create(p).table("person");

//    auto res1 = q1.execute<person>();
//    auto res2 = q2.execute();
//    auto res3 = q3.execute<person>();

//    auto &q4 = c.create_query().create(p).select(p);

  } catch (const std::exception &ex) {
    std::cout << "caught exception: " << ex.what() << "\n";
  }

}