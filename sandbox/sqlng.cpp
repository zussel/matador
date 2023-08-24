#include <functional>
#include <iostream>
#include <string>
#include <vector>

class sql_dialect;

template < class T, template < class V > class R, typename std::enable_if<!std::is_same<T, std::string>::value>::type* = nullptr>
std::string join(const R<T> &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (const auto &i : range) {
      result += to_string(i);
    }
  } else {
    auto it = range.begin();
    result += to_string(*it++);
    for (;it != range.end(); ++it) {
      result += delim + to_string(*it);
    }
  }
  return result;
}

template < template < class V > class R >
std::string join(const R<std::string> &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (const auto &i : range) {
      result += i;
    }
  } else {
    auto it = range.begin();
    result += *it++;
    for (;it != range.end(); ++it) {
      result += delim + *it;
    }
  }
  return result;
}

struct token
{
  enum class token_part {
    UNKNOWN, CREATE, DROP, SELECT, INSERT, UPDATE, REMOVE,
    TABLE, COLUMNS, FROM, INTO, SET, WHERE, ORDER_BY, ASC, DESC,
    OFFSET, LIMIT, VALUES, BEGIN, COMMIT, ROLLBACK
  };

  std::string build(sql_dialect &dialect) const;

  token_part part { token_part::UNKNOWN };
  std::function<std::string(sql_dialect&)> builder {};
  bool enabled { true };
};

class sql_dialect
{
public:
  virtual ~sql_dialect() = default;

  template<token::token_part TokenPart>
  static token phrase();

  virtual std::string create() const { return "CREATE"; }
  virtual std::string drop() const { return "DROP"; }
  virtual std::string select() const { return "SELECT"; }
  virtual std::string insert() const { return "INSERT"; }
  virtual std::string update() const { return "UPDATE"; }
  virtual std::string remove() const { return "DELETE"; }
  virtual std::string table() const { return "TABLE"; }
  virtual std::string from() const { return "FROM"; }
  virtual std::string where() const { return "WHERE"; }
  virtual std::string into() const { return "INTO"; }
  virtual std::string set() const { return "SET"; }
  virtual std::string order_by() const { return "ORDER BY"; }
  virtual std::string asc() const { return "ASC"; }
  virtual std::string desc() const { return "DESC"; }
  virtual std::string values() const { return "VALUES"; }
  virtual std::string offset() const { return "OFFSET"; }
  virtual std::string limit() const { return "LIMIT"; }
  virtual std::string not_null() const { return "NOT NULL"; }
  virtual std::string primary_key() const { return "PRIMARY KEY"; }
  virtual std::string distinct() const { return "DISTINCT"; }

  virtual std::string begin() const { return "BEGIN TRANSACTION"; }
  virtual std::string commit() const { return "COMMIT TRANSACTION"; }
  virtual std::string rollback() const { return "ROLLBACK TRANSACTION"; }

  virtual char start_quote() const { return '"'; }
  virtual char end_quote() const { return '"'; }
  virtual char string_quote() const { return '\''; }

  virtual std::string char_type() const { return "CHAR(1)"; }
  virtual std::string short_type() const { return "SMALLINT"; }
  virtual std::string int_type() const { return "INTEGER"; }
  virtual std::string long_type() const { return "BIGINT"; }
  virtual std::string long_long_type() const { return "BIGINT"; }
  virtual std::string unsigned_char_type() const { return "TINYINT"; }
  virtual std::string unsigned_short_type() const { return "SMALLINT"; }
  virtual std::string unsigned_int_type() const { return "INTEGER"; }
  virtual std::string unsigned_long_type() const { return "BIGINT"; }
  virtual std::string unsigned_long_long_type() const { return "BIGINT"; }
  virtual std::string boolean_type() const { return "BOOLEAN"; }
  virtual std::string float_type() const { return "FLOAT"; }
  virtual std::string double_type() const { return "DOUBLE"; }
  virtual std::string string_type(int size) const { return (size == -1 ? "TEXT" : sql_dialect::varchar("VARCHAR", size)); }

  virtual void on_update_delete_limit(token &where_token, const std::string &table_name, size_t limit) {
    auto builder = where_token.builder;
    where_token.builder = [&table_name, limit, builder](sql_dialect &dialect) {
      return " " + dialect.where() + " rowid IN (" + dialect.select() + " rowid " + dialect.from() + " " + table_name + " " + builder(dialect) + " " + dialect.limit() + " " + std::to_string(limit) + ")";
    };
  }

private:
  static std::string varchar(const char *vc, int size)
  {
    return vc + std::string("(") + std::to_string(size) + ")";
  }
};

template<>
token sql_dialect::phrase<token::token_part::CREATE>() { return { token::token_part::CREATE, [](sql_dialect &dialect) { return dialect.create(); } }; }
template<>
token sql_dialect::phrase<token::token_part::DROP>() { return { token::token_part::DROP, [](sql_dialect &dialect) { return dialect.drop(); } }; }
template<>
token sql_dialect::phrase<token::token_part::SELECT>() { return { token::token_part::SELECT, [](sql_dialect &dialect) { return dialect.select(); } }; }
template<>
token sql_dialect::phrase<token::token_part::INSERT>() { return { token::token_part::INSERT, [](sql_dialect &dialect) { return dialect.insert(); } }; }
template<>
token sql_dialect::phrase<token::token_part::UPDATE>() { return { token::token_part::UPDATE, [](sql_dialect &dialect) { return dialect.update(); } }; }
template<>
token sql_dialect::phrase<token::token_part::REMOVE>() { return { token::token_part::REMOVE, [](sql_dialect &dialect) { return dialect.remove(); } }; }
template<>
token sql_dialect::phrase<token::token_part::INTO>() { return { token::token_part::INTO, [](sql_dialect &dialect) { return dialect.into(); } }; }
template<>
token sql_dialect::phrase<token::token_part::VALUES>() { return { token::token_part::VALUES, [](sql_dialect &dialect) { return dialect.values(); } }; }
template<>
token sql_dialect::phrase<token::token_part::LIMIT>() { return { token::token_part::LIMIT, [](sql_dialect &dialect) { return dialect.limit(); } }; }
template<>
token sql_dialect::phrase<token::token_part::ASC>() { return { token::token_part::ASC, [](sql_dialect &dialect) { return " " + dialect.asc(); } }; }
template<>
token sql_dialect::phrase<token::token_part::DESC>() { return { token::token_part::DESC, [](sql_dialect &dialect) { return " " + dialect.desc(); } }; }

std::string token::build(sql_dialect &dialect) const
{
  return builder(dialect);
}

template < class T, typename = void >
struct field_type_traits;

template < class T >
struct field_type_traits<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
  static std::string type(sql_dialect &dialect) { return dialect.int_type(); }
};

template <> struct field_type_traits<char>
{
  static std::string type(sql_dialect &dialect) { return dialect.char_type(); }
};

template <> struct field_type_traits<short>
{
  static std::string type(sql_dialect &dialect) { return dialect.short_type(); }
};

template <> struct field_type_traits<int>
{
  static std::string type(sql_dialect &dialect) { return dialect.int_type(); }
};

template <> struct field_type_traits<long>
{
  static std::string type(sql_dialect &dialect) { return dialect.long_type(); }
};

template <> struct field_type_traits<long long>
{
  static std::string type(sql_dialect &dialect) { return dialect.long_long_type(); }
};

template <> struct field_type_traits<unsigned char>
{
  static std::string type(sql_dialect &dialect) { return dialect.char_type(); }
};

template <> struct field_type_traits<unsigned short>
{
  static std::string type(sql_dialect &dialect) { return dialect.short_type(); }
};

template <> struct field_type_traits<unsigned int>
{
  static std::string type(sql_dialect &dialect) { return dialect.int_type(); }
};

template <> struct field_type_traits<unsigned long>
{
  static std::string type(sql_dialect &dialect) { return dialect.long_type(); }
};

template <> struct field_type_traits<unsigned long long>
{
  static std::string type(sql_dialect &dialect) { return dialect.long_long_type(); }
};

template <> struct field_type_traits<bool>
{
  static std::string type(sql_dialect &dialect) { return dialect.boolean_type(); }
};

template <> struct field_type_traits<float>
{
  static std::string type(sql_dialect &dialect) { return dialect.float_type(); }
};

template <> struct field_type_traits<double>
{
  static std::string type(sql_dialect &dialect) { return dialect.double_type(); }
};

template <> struct field_type_traits<std::string>
{
  static std::string type(sql_dialect &dialect, int size = -1) { return dialect.string_type(size); }
};

template < class Operator, class Type >
void process(Operator &op, Type &object)
{
  object.process(op);
}

template < class Operator, class T >
void primary_key(Operator &op, const char *id, T &value)
{
  op.on_primary_key(id, value);
}

template < class Operator >
void revision(Operator &op, const char *id, unsigned long long &value)
{
  op.on_revision(id, value);
}

template < class Operator, class T >
void attribute(Operator &op, const char *id, T &value, int size = -1)
{
  op.on_attribute(id, value, size);
}

struct column_serializer
{
  enum class serialize_type {
    COLUMNS, VALUES, SETTER, CREATE
  };

  explicit column_serializer(sql_dialect &dialect)
  : dialect_(dialect)
  {}

  template < class Type >
  std::string create(const Type &object)
  {
    prepare(serialize_type::CREATE);
    process(*this, const_cast<Type&>(object));
    return result;
  }

  template < class Type >
  std::string columns(const Type &object)
  {
    prepare(serialize_type::COLUMNS);
    process(*this, const_cast<Type&>(object));
    return result;
  }

  template < class Type >
  std::string values(const Type &object)
  {
    prepare(serialize_type::VALUES);
    process(*this, const_cast<Type&>(object));
    return result;
  }

  template < class Type >
  std::string setter(const Type &object)
  {
    prepare(serialize_type::SETTER);
    process(*this, const_cast<Type&>(object));
    return result;
  }

  template < class T >
  void on_primary_key(const char *id, T &pk)
  {
    check_first();
    switch (type_) {
      case serialize_type::CREATE:
        result.append(id);
        result.append(" ");
        result += field_type_traits<T>::type(dialect_);
        result += " " + dialect_.not_null() + " " + dialect_.primary_key();
        break;
      case serialize_type::COLUMNS:
        result.append(id);
        break;
      case serialize_type::VALUES:
        result += std::to_string(pk);
        break;
      case serialize_type::SETTER:
        result.append(id);
        result.append("=");
        result += std::to_string(pk);
        break;
    }
  }

  void on_revision(const char *id, unsigned long long revision)
  {
    check_first();
    switch (type_) {
      case serialize_type::CREATE:
        result.append(id);
        result.append(" ");
        result += field_type_traits<unsigned long long>::type(dialect_);
        break;
      case serialize_type::COLUMNS:
        result.append(id);
        break;
      case serialize_type::VALUES:
        result += std::to_string(revision);
        break;
      case serialize_type::SETTER:
        result.append(id);
        result.append("=");
        result += std::to_string(revision);
        break;
    }
  }
  template < class T >
  void on_attribute(const char *id, T &value, int)
  {
    check_first();
    switch (type_) {
      case serialize_type::CREATE:
        result.append(id);
        result.append(" ");
        result += field_type_traits<T>::type(dialect_);
        break;
      case serialize_type::COLUMNS:
        result.append(id);
        break;
      case serialize_type::VALUES:
        result += std::to_string(value);
        break;
      case serialize_type::SETTER:
        result.append(id);
        result.append("=");
        result += std::to_string(value);
        break;
    }
  }

  void on_attribute(const char *id, std::string &value, int size = -1)
  {
    check_first();
    switch (type_) {
      case serialize_type::CREATE:
        result.append(id);
        result.append(" ");
        result += field_type_traits<std::string>::type(dialect_, size);
        break;
      case serialize_type::COLUMNS:
        result.append(id);
        break;
      case serialize_type::VALUES:
        result += value;
        break;
      case serialize_type::SETTER:
        result.append(id);
        result.append("=");
        result += value;
        break;
    }
  }

  void check_first()
  {
    if (!first_) {
      result.append(", ");
    } else {
      first_ = false;
    }
  }

  void prepare(serialize_type type)
  {
    result.clear();
    first_ = true;
    type_ = type;
  }

  std::string result;

  sql_dialect &dialect_;
  bool first_{true};
  serialize_type type_{serialize_type::COLUMNS};
};

struct mysql_dialect : sql_dialect
{
  std::string update() const override { return "MODIFY"; }
};

template < class Type >
class query
{
public:
  /**
   * Schema:
   *
   * query<Type>.create().table(<table name>);
   *
   * @return
   */
  query& create()
  {
    initialize<token::token_part::CREATE>();
    return *this;
  }

  /**
   * Schema:
   *
   * query<Type>.drop().table(<table name>);
   *
   * @return
   */
  query& drop()
  {
    initialize<token::token_part::DROP>();
    return *this;
  }

  query& table(const std::string &table_name)
  {
    table_name_ = table_name;

    if (is_drop()) {
      tokens_.push_back({token::token_part::TABLE, [this](sql_dialect &dialect) {
        return " " + dialect.table() + " " + table_name_;
      }});
    } else {
      tokens_.push_back({token::token_part::TABLE, [this](sql_dialect &dialect) {
        column_serializer cs(dialect);
        return " " + dialect.table() + " " + table_name_ + " (" + cs.create(prototype_) + ")";
      }});
    }
    return *this;
  }

  /**
   * Schema:
   *
   * query<Type>.select(<fields/object>).from(<table>)[.where(<condition>)][.order_by(<order>)][.asc()|.desc()][.offset(<offset>)][.limit(<limit>)];
   *
   * Example:
   *
   * query<person> q;
   *
   * selects all persons (all fields) from table person
   * q.select().from("person").where("name"_col == "george");
   *
   * selects all persons from table person order by id asc with offset 30 and limit 10
   * q.select().from("person").order_by("id"_col).asc().offset(30).limit(10);
   *
   * @return
   */
  query& select()
  {
    initialize<token::token_part::SELECT>();
    tokens_.push_back({token::token_part::COLUMNS, [this](sql_dialect &dialect) {
      column_serializer cs(dialect);
      return " " + cs.columns(prototype_);
    }});
    return *this;
  }

  query& select(const std::initializer_list<std::string> &column_names)
  {
    initialize<token::token_part::SELECT>();
    tokens_.push_back({token::token_part::COLUMNS, [column_names](sql_dialect &dialect) {
      return " " + join(column_names, ",");
    }});
    // Todo: add columns
    return *this;
  }

  /**
   * Schema:
   *
   * query<Type>.insert().into(<table>).values(<object>);
   *
   * @return
   */
  query& insert()
  {
    initialize<token::token_part::INSERT>();
    return *this;
  }

  /**
   * Schema:
   *
   * query<Type>.update(<table>).set(<object>)[.where(<condition>)];
   *
   * @return
   */
  query& update(const std::string &table_name)
  {
    initialize<token::token_part::UPDATE>(table_name);
    return *this;
  }

  /**
   * Schema:
   *
   * query<Type>.remove().from(<table>)[.where(<condition>)];
   *
   * @return
   */
  query& remove()
  {
    initialize<token::token_part::REMOVE>();
    return *this;
  }

  query& values(const Type &object)
  {
    tokens_.push_back({token::token_part::VALUES, [&object](sql_dialect &dialect) {
      column_serializer cs(dialect);
      return " " + dialect.values() + " " + cs.values(object);
    }});
    return *this;
  }

  query& from(const std::string &table_name)
  {
    table_name_ = table_name;
    tokens_.push_back({token::token_part::FROM, [this](sql_dialect &dialect) {
      return " " + dialect.from() + " " + table_name_;
    }});
    return *this;
  }

  query& into(const std::string &table_name)
  {
    table_name_ = table_name;
    tokens_.push_back({token::token_part::INTO, [this](sql_dialect &dialect) {
      column_serializer cs(dialect);
      return " " + dialect.into() + " " + table_name_ + " (" + cs.columns(prototype_) + ")";
    }});
    return *this;
  }

  query& set(const Type& object)
  {
    tokens_.push_back({token::token_part::INTO, [&object](sql_dialect &dialect) {
      column_serializer cs(dialect);
      return " " + dialect.set() + " " + cs.setter(object);
    }});
    return *this;
  }

  template < class COND >
  query& where(const COND &c)
  {
    tokens_.push_back({token::token_part::WHERE, [](sql_dialect &dialect) {
      return " " + dialect.where();
    }});
    return *this;
  }

  query& where(const char *cond)
  {
    return where(std::string(cond));
  }

  query& where(const std::string &cond)
  {
    if (sql_command_ == token::token_part::UPDATE || sql_command_ == token::token_part::REMOVE) {
      // store index of where for possible later
      // modification
      where_index_ = tokens_.size();
    }
    tokens_.push_back({token::token_part::WHERE, [cond](sql_dialect &dialect) {
      return " " + dialect.where() + " " + cond;
    }});
    return *this;
  }

  query& order_by(const std::string &col)
  {
    tokens_.push_back({token::token_part::ORDER_BY, [col](sql_dialect &dialect) {
      return " " + dialect.order_by() + " " + col;
    }});
    return *this;
  }
  query& asc()
  {
    tokens_.push_back(sql_dialect::phrase<token::token_part::ASC>());
    return *this;
  }
  query& desc()
  {
    tokens_.push_back(sql_dialect::phrase<token::token_part::DESC>());
    return *this;
  }

  query& offset(size_t o)
  {
    tokens_.push_back({token::token_part::OFFSET, [o](sql_dialect &dialect) {
      return " " + dialect.offset() + " " + std::to_string(o);
    }});
    return *this;
  }

  query& limit(size_t l)
  {
    limit_ = l;
    tokens_.push_back({token::token_part::LIMIT, [this](sql_dialect &dialect) {
      return " " + dialect.limit() + " " + std::to_string(limit_);
    }});
    return *this;
  }

  std::string str(sql_dialect &dialect)
  {
    for (auto &tk : tokens_) {
      switch (tk.part) {
        case token::token_part::LIMIT:
          on_limit(tk, dialect);
          break;
        default:
          break;
      }
    }
    std::string result;
    for (const auto &tk : tokens_) {
      if (!tk.enabled) {
        continue;
      }
      result += tk.build(dialect);
    }
    return result;
  }

  bool is_drop() const
  {
    return sql_command_ == token::token_part::DROP;
  }

  bool is_update() const
  {
    return sql_command_ == token::token_part::UPDATE;
  }

  bool is_delete() const
  {
    return sql_command_ == token::token_part::REMOVE;
  }

private:
  void on_limit(token &limit_token, sql_dialect &dialect)
  {
    if (is_delete() || is_update()) {
      dialect.on_update_delete_limit(tokens_[where_index_], table_name_, limit_);
      limit_token.enabled = false;
    }
  }

  template<token::token_part TokenPart>
  void initialize()
  {
    tokens_ = { sql_dialect::phrase<TokenPart>() };
    sql_command_ = TokenPart;
  }

  template<token::token_part TokenPart>
  void initialize(const std::string& table_name)
  {
    table_name_ = table_name;
    tokens_ = { { TokenPart, [table_name](sql_dialect &dialect) {
      return dialect.update() + " " + table_name;
    } } };
    sql_command_ = TokenPart;
  }

private:
  Type prototype_;

  token::token_part sql_command_ { token::token_part::UNKNOWN };
  std::string table_name_;
  size_t where_index_ {};
  size_t limit_{};

  std::vector<token> tokens_;
};

namespace sql {

template < class Type >
query<Type> create()
{
  query<Type> q;
  return q.create();
}

template < class Type >
query<Type> drop()
{
  query<Type> q;
  return q.drop();
}

template < class Type >
query<Type> select()
{
  query<Type> q;
  return q.select();
}

template < class Type >
query<Type> insert()
{
  query<Type> q;
  return q.insert();
}

template < class Type >
query<Type> update(const std::string& table_name)
{
  query<Type> q;
  return q.update(table_name);
}

template < class Type >
query<Type> remove()
{
  query<Type> q;
  return q.remove();
}

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

class client
{
public:

  template<class Type>
  result<Type> execute(const query<Type>& q)
  {
    return {};
  }

  template<class Type>
  statement<Type> prepare(const query<Type>)
  {
    return {};
  }

};

}


struct person
{
  enum gender_type {
    unknown, male, female, diverse
  };

  unsigned long id {};
  unsigned long long version {};
  std::string name{};
  unsigned int height{};
  unsigned int weight{};
  gender_type gender{unknown};

  template<class Operator>
  void process(Operator &op)
  {
    primary_key(op, "id", id);
    revision(op, "version", version);
    attribute(op, "name", name, 255);
    attribute(op, "height", height);
    attribute(op, "weight", weight);
    attribute(op, "gender", gender);
  }
};

int main()
{
  mysql_dialect dialect;
  query<person> q;

  q.create().table("t_person");

  std::cout << q.str(dialect) << "\n";

  person p { 13UL, 4711, "george", 183, 75, person::gender_type::male };

  q = sql::insert<person>().into("t_person").values(p);

  std::cout << q.str(dialect) << "\n";

  p.name = "henry";
  p.height = 178;

  auto qu = sql::update<person>("t_person").set(p).where("id > 0");

  std::cout << qu.str(dialect) << "\n";

  q.update("t_person").set(p).where("id > 0");

  std::cout << q.str(dialect) << "\n";

  q.update("t_person").set(p).where("id > 0").limit(1);

  std::cout << q.str(dialect) << "\n";

  q = sql::select<person>().from("t_person").where("name = 'george'");

  std::cout << q.str(dialect) << "\n";

  q = sql::select<person>().from("t_person").where("height > 75").order_by("birthdate").asc().offset(10).limit(5);

  std::cout << q.str(dialect) << "\n";

  q = sql::remove<person>().from("t_person").where("name = 'george'");

  std::cout << q.str(dialect) << "\n";

  q = sql::drop<person>().table("t_person");

  std::cout << q.str(dialect) << "\n";

//  column_serializer cs(dialect);

//  std::cout << "select columns: " << cs.columns(p) << "\n";
//  std::cout << "values: " << cs.values(p) << "\n";
//  std::cout << "setter: " << cs.setter(p) << "\n";
//  std::cout << "create: " << cs.create(p) << "\n";

  return 0;
}