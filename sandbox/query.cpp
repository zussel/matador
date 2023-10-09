#include <iostream>
#include <memory>
#include <utility>

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

class query
{
public:
  explicit query(connection &c) {}

  template<class Type>
  query& create() {
    const Type t;
    return create(t);
  };

  template<class Type>
  query& create(const Type &t) { return *this; };
  query& create(const std::initializer_list<std::shared_ptr<column>> &/*column_list*/) { return *this; };
  query& create(const row &/*r*/) { return *this; };

  query& drop() { return *this; };
  query& table(const std::string &table_name) { return *this; }

  query& select() { return *this; }
  query& from(const std::string &table_name) { return *this; }
  query& where() { return *this; }

  query& insert() { return *this; }
  query& into(const std::string &table_name) { return *this; }
  query& values() { return *this; }

  query& update(const std::string &table_name) { return *this; }
  query& set() { return *this; }

  query& remove() { return *this; }

  template <class Type>
  result<Type> execute() { return {}; }
  result<row> execute() { return {}; }
  template <class Type>
  statement<Type> prepare() { return {}; }
  statement<row> prepare() { return {}; }

private:
};

class connection
{
public:
  query create_query() { return query{*this}; }

};

struct person{};
int main() {


  connection c;

  person p;
  auto& q1 = c.create_query().create<person>().table("person");
  auto& q2 = c.create_query().create({ std::make_shared<column>("id")}).table("person");
  auto& q3 = c.create_query().create(p).table("person");

  auto res1 = q1.execute<person>();
  auto res2 = q2.execute();
  auto res3 = q2.execute<person>();

}