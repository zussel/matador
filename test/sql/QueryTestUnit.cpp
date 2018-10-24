//
// Created by sascha on 9/7/15.
//

#include "QueryTestUnit.hpp"

#include "../Item.hpp"
#include "../person.hpp"

#include "matador/sql/query.hpp"

#include <algorithm>
#include <set>

using namespace matador;

QueryTestUnit::QueryTestUnit(const std::string &name, const std::string &msg, const std::string &db, const matador::time &timeval)
  : unit_test(name, msg)
  , db_(db)
  , time_val_(timeval)
{
  add_test("datatypes", std::bind(&QueryTestUnit::test_datatypes, this), "test sql datatypes");
  add_test("qvc", std::bind(&QueryTestUnit::test_query_value_creator, this), "test query value creator");
  add_test("quoted_identifier", std::bind(&QueryTestUnit::test_quoted_identifier, this), "test quoted identifier");
  add_test("columns_with_quotes", std::bind(&QueryTestUnit::test_columns_with_quotes_in_name, this), "test columns with quotes in name");
  add_test("quoted_literals", std::bind(&QueryTestUnit::test_quoted_literals, this), "test quoted literals");
  add_test("bind_tablename", std::bind(&QueryTestUnit::test_bind_tablename, this), "test bind tablenames");
  add_test("describe", std::bind(&QueryTestUnit::test_describe, this), "test describe table");
  add_test("identifier", std::bind(&QueryTestUnit::test_identifier, this), "test sql identifier");
  add_test("update", std::bind(&QueryTestUnit::test_update, this), "test direct sql update statement");
  add_test("create", std::bind(&QueryTestUnit::test_create, this), "test direct sql create statement");
  add_test("create_anonymous", std::bind(&QueryTestUnit::test_anonymous_create, this), "test direct sql create statement via row (anonymous)");
  add_test("insert_anonymous", std::bind(&QueryTestUnit::test_anonymous_insert, this), "test direct sql insert statement via row (anonymous)");
  add_test("update_anonymous", std::bind(&QueryTestUnit::test_anonymous_update, this), "test direct sql update statement via row (anonymous)");
  add_test("statement_insert", std::bind(&QueryTestUnit::test_statement_insert, this), "test prepared sql insert statement");
  add_test("statement_update", std::bind(&QueryTestUnit::test_statement_update, this), "test prepared sql update statement");
  add_test("delete", std::bind(&QueryTestUnit::test_delete, this), "test query delete");
//  add_test("foreign_query", std::bind(&QueryTestUnit::test_foreign_query, this), "test query with foreign key");
  add_test("query", std::bind(&QueryTestUnit::test_query, this), "test query");
  add_test("result_range", std::bind(&QueryTestUnit::test_query_range_loop, this), "test result range loop");
  add_test("select", std::bind(&QueryTestUnit::test_query_select, this), "test query select");
  add_test("select_count", std::bind(&QueryTestUnit::test_query_select_count, this), "test query select count");
  add_test("select_columns", std::bind(&QueryTestUnit::test_query_select_columns, this), "test query select columns");
  add_test("select_limit", std::bind(&QueryTestUnit::test_select_limit, this), "test query select limit");
  add_test("update_limit", std::bind(&QueryTestUnit::test_update_limit, this), "test query update limit");
  add_test("prepared_statement", std::bind(&QueryTestUnit::test_prepared_statement, this), "test query prepared statement");
  add_test("prepared_statement_creation", std::bind(&QueryTestUnit::test_prepared_statement_creation, this), "test query prepared statement creation");
  add_test("object_result_twice", std::bind(&QueryTestUnit::test_prepared_object_result_twice, this), "test query prepared statement get object result twice");
  add_test("scalar_result_twice", std::bind(&QueryTestUnit::test_prepared_scalar_result_twice, this), "test query prepared statement get scalar result twice");
  add_test("rows", std::bind(&QueryTestUnit::test_rows, this), "test row value serialization");
}

template < class C, class T >
bool contains(const C &container, const T &value)
{
  return std::find(std::begin(container), std::end(container), value) != std::end(container);
}

void QueryTestUnit::initialize()
{
  connection_ = create_connection();
}

void QueryTestUnit::test_datatypes()
{
  connection_.open();

  query<Item> q("item");

  q.create().execute(connection_);

  float fval = 2.445566f;
  double dval = 11111.23433345;
  char cval = 'c';
  short sval = -128;
  int ival = -49152;
  long lval = -123456789;
  unsigned short usval = 255;
  unsigned int uival = 49152;
  unsigned long ulval = 765432182;
  bool bval = true;
  const char *cstr("Armer schwarzer Kater");
  matador::varchar<32> vval("hallo welt");
  std::string strval = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
  matador::date date_val(15, 3, 2015);
  matador::time time_val(time_val_);
  Item item;

  // set values
  item.set_bool(bval);
  item.set_char(cval);
  item.set_double(dval);
  item.set_float(fval);
  item.set_short(sval);
  item.set_int(ival);
  item.set_long(lval);
  item.set_unsigned_short(usval);
  item.set_unsigned_int(uival);
  item.set_unsigned_long(ulval);
  item.set_cstr(cstr, strlen(cstr) + 1);
  item.set_varchar(vval);
  item.set_string(strval);
  item.set_date(date_val);
  item.set_time(time_val);

  q.insert(item).execute(connection_);

  auto res = q.select().execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last, "first must not be last");

  std::unique_ptr<Item> it((first++).release());

  UNIT_EXPECT_EQUAL(it->get_char(), cval, "character is not equal");
  UNIT_EXPECT_EQUAL(it->get_short(), sval, "short is not equal");
  UNIT_EXPECT_EQUAL(it->get_int(), ival, "integer is not equal");
  UNIT_EXPECT_EQUAL(it->get_long(), lval, "long is not equal");
  UNIT_EXPECT_EQUAL(it->get_unsigned_short(), usval, "unsigned short is not equal");
  UNIT_EXPECT_EQUAL(it->get_unsigned_int(), uival, "unsigned integer is not equal");
  UNIT_EXPECT_EQUAL(it->get_unsigned_long(), ulval, "unsigned long is not equal");
  UNIT_EXPECT_EQUAL(it->get_bool(), bval, "bool is not equal");
  UNIT_EXPECT_EQUAL(it->get_cstr(), cstr, "const char pointer is not equal");
  UNIT_EXPECT_EQUAL(it->get_string(), strval, "strings is not equal");
  UNIT_EXPECT_EQUAL(it->get_varchar(), vval, "varchar is not equal");
  UNIT_EXPECT_EQUAL(it->get_date(), date_val, "date is not equal");
  UNIT_EXPECT_EQUAL(it->get_time(), time_val, "time is not equal");

  q.drop().execute(connection_);
}

void QueryTestUnit::test_query_value_creator()
{
  connection_.open();

  matador::detail::query_value_creator qvc;

  matador::any ac = 'c';

  auto val = qvc.create_from_any(ac);

  UNIT_ASSERT_EQUAL(val->get<char>(), 'c', "values must be equal");
}

void QueryTestUnit::test_quoted_identifier()
{
  connection_.open();

  query<> q(connection_, "quotes");

  q.create({make_typed_column<std::string>("from"), make_typed_column<std::string>("to")}).execute();

  // check table description
  std::vector<std::string> columns = { "from", "to"};
  std::vector<data_type > types = { matador::data_type::type_text, matador::data_type::type_text};
  auto fields = connection_.describe("quotes");

  for (auto &&field : fields) {
    UNIT_EXPECT_EQUAL(field.name(), columns[field.index()], "invalid column name");
    UNIT_EXPECT_EQUAL((int)field.type(), (int)types[field.index()], "invalid column type");
  }

  q.insert({"from", "to"}).values({"Berlin", "London"}).execute();

  auto res = q.select({"from", "to"}).from("quotes").execute();

  for (auto row : res) {
    UNIT_EXPECT_EQUAL("Berlin", row->at<std::string>("from"), "values must be equal");
    UNIT_EXPECT_EQUAL("London", row->at<std::string>("to"), "values must be equal");
  }

  column from("from");
  q.update({{"from", "Hamburg"}, {"to", "New York"}}).where(from == "Berlin").execute();

  res = q.select({"from", "to"}).from("quotes").execute();

  for (auto row : res) {
    UNIT_EXPECT_EQUAL("Hamburg", row->at<std::string>("from"), "values must be equal");
    UNIT_EXPECT_EQUAL("New York", row->at<std::string>("to"), "values must be equal");
  }

  q.drop().execute();
}

void QueryTestUnit::test_columns_with_quotes_in_name()
{
  connection_.open();

  char opening_char = connection_.dialect()->identifier_opening_quote();
  char closing_char = connection_.dialect()->identifier_closing_quote();
  std::stringstream column_name;
  column_name << "name_with_" << opening_char << "open_close_quotes" << closing_char << "_in_backend_ctx";

  std::vector<std::string> colnames = {
    "normal_name",
    column_name.str(),
    "name_with_'string'_\"literal\"_quotes",
    "name_with_`identifier_quotes`_in_backend_ctx",
    "from"
  };

  for (const auto &colname : colnames) {
    query<> q(connection_, "quotes");

    q.create({make_typed_column<std::string>(colname)}).execute();

    // check table description
    std::vector<std::string> columns({ colname });
    std::vector<data_type > types({ matador::data_type::type_text });
    auto fields = connection_.describe("quotes");

    for (auto &&field : fields) {
      UNIT_EXPECT_EQUAL(field.name(), columns[field.index()], "invalid column name");
      UNIT_EXPECT_EQUAL((int)field.type(), (int)types[field.index()], "invalid column type");
    }

    q.drop().execute();
  }
}

void QueryTestUnit::test_quoted_literals()
{
  connection_.open();

  query<> q(connection_, "escapes");

  q.create({make_typed_column<std::string>("name")}).execute();

  q.insert({"name"}).values({"text"}).execute();

  auto res = q.select({"name"}).from("escapes").execute();

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text", "values must be equal");
  }

  q.update({{"name", "text'd"}}).execute();

  res = q.select({"name"}).from("escapes").execute();

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text'd", "values must be equal");
  }

  q.update({{"name", "text\nhello\tworld"}}).execute();

  res = q.select({"name"}).from("escapes").execute();

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text\nhello\tworld", "values must be equal");
  }

  q.update({{"name", "text \"text\""}}).execute();

  res = q.select({"name"}).from("escapes").execute();

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text \"text\"", "values must be equal");
  }

  q.drop().execute();
}

void QueryTestUnit::test_bind_tablename()
{
  matador::query<person>::clear_bound_tables();

  matador::query<person> q0; // tablename should be person

  UNIT_ASSERT_TRUE(q0.tablename().empty(), "name must be empty");
  matador::query<person>::bind_table("person");

  matador::query<person> q1; // tablename should be person

  UNIT_ASSERT_EQUAL(q1.tablename(), "person", "names must be equal");

  matador::query<person> q2("student"); // tablename should be student

  UNIT_ASSERT_EQUAL(q2.tablename(), "student", "names must be equal");
}

void QueryTestUnit::test_describe()
{
  connection_.open();

  matador::query<person> q("person");

  q.create().execute(connection_);

  auto fields = connection_.describe("person");

  std::vector<std::string> columns = { "id", "name", "birthdate", "height"};
  std::vector<data_type > types = { matador::data_type::type_long, matador::data_type::type_varchar, matador::data_type::type_date, matador::data_type::type_long};

  for (auto &&field : fields) {
    UNIT_ASSERT_EQUAL(field.name(), columns[field.index()], "invalid column name");
    UNIT_ASSERT_EQUAL((int)field.type(), (int)types[field.index()], "invalid column type");
  }

  q.drop().execute(connection_);
}

class pktest
{
public:
  pktest() = default;
  pktest(unsigned long i, std::string n)
    : id(i), name(std::move(std::move(n)))
  {}

  ~pktest() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
  }

  matador::identifier<unsigned long> id;
  std::string name;
};

void QueryTestUnit::test_identifier()
{
  connection_.open();

  matador::query<pktest> q("pktest");

  q.create().execute(connection_);

  std::unique_ptr<pktest> p(new pktest(7, "hans"));

  UNIT_EXPECT_EQUAL(p->id.value(), 7UL, "identifier value should be greater zero");

  q.insert(*p).execute(connection_);

  auto res = q.select().execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last, "first must not be last");

  p.reset((first++).release());

  UNIT_EXPECT_GREATER(p->id.value(), 0UL, "identifier value should be greater zero");

  q.drop().execute(connection_);
}

void QueryTestUnit::test_create()
{
  connection_.open();

  query<Item> q("item");

  q.create().execute(connection_);

  auto itime = time_val_;
  Item hans("Hans", 4711);
  hans.set_time(itime);
  result<Item> res = q.insert(hans).execute(connection_);

  res = q.select().execute(connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_EXPECT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_EXPECT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_EXPECT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  res = q.drop().execute(connection_);
}

void QueryTestUnit::test_update()
{
  connection_.open();

  query<person> q(connection_, "person");

  // create item table and insert item
  result<person> res(q.create().execute());

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto &name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    q.insert(p).execute();
  }

  column name("name");
  res = q.select().where(name == "hans").execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  person hans("hans", matador::date(15, 6, 1990), 165);
  hans.id(1);
  column idcol("id");
  q.update(hans).where(idcol == 1).execute();

  res = q.select().where(name == "hans").execute();

  for (auto i : res) {
    UNIT_ASSERT_EQUAL(i->name(), "hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(i->height(), 165U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(i->birthdate(), matador::date(15, 6, 1990), "expected birthdate is 12.3.1980");
  }

  q.drop().execute();
}

void QueryTestUnit::test_anonymous_create()
{
  connection_.open();

  query<> q(connection_, "person");

  auto cols = {"id", "name", "age"};

  q.create({
    make_typed_id_column<long>("id"),
    make_typed_varchar_column<32>("name"),
    make_typed_column<unsigned>("age")
  });

  q.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"), "table person must exist");
  auto fields = connection_.describe("person");

  for (const auto &fld : fields) {
    UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end(), "couldn't find expected field");
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_anonymous_insert()
{
  connection_.open();

  query<> q(connection_, "person");

  q.create({
     make_typed_id_column<long>("id"),
     make_typed_varchar_column<32>("name"),
     make_typed_column<unsigned>("age")
   });

  q.execute();

  q.insert({"id", "name", "age"}).values({1, "hans", 45}).execute();

  auto res = q.select({"id", "name", "age"}).from("person").execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(1L, item->at<long>("id"), "invalid value");
    UNIT_EXPECT_EQUAL("hans", item->at<std::string>("name"), "invalid value");
    ++first;
  }

  q.drop("person").execute(connection_);

}

void QueryTestUnit::test_anonymous_update()
{
  connection_.open();

  query<> q(connection_, "person");

  q.create({
     make_typed_id_column<long>("id"),
     make_typed_varchar_column<32>("name"),
     make_typed_column<unsigned>("age")
   });

  q.execute();

  q.insert({"id", "name", "age"}).values({1, "hans", 45}).execute();

  column name("name");

  q.update({{"name", "jane"}, {"age", 47}}).where(name == "hans").execute();

  auto res = q.select({"id", "name", "age"}).from("person").execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL("jane", item->at<std::string>("name"), "invalid value");
    UNIT_EXPECT_EQUAL(47L, item->at<long>("age"), "invalid value");
    ++first;
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_statement_insert()
{
  connection_.open();

  query<Item> q("item");

  statement<Item> stmt(q.create().prepare(connection_));

  result<Item> res(stmt.execute());

  matador::identifier<unsigned long> id(23);
  auto itime = time_val_;
  Item hans("Hans", 4711);
  hans.id(id.value());
  hans.set_time(itime);
  stmt = q.insert(hans).prepare(connection_);

  stmt.bind(0, &hans);
  res = stmt.execute();

  stmt = q.select().prepare(connection_);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_EXPECT_EQUAL(item->id(), 23UL, "expected id must be 23");
    UNIT_EXPECT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_EXPECT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_EXPECT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  stmt = q.drop().prepare(connection_);

  res = stmt.execute();
}

void QueryTestUnit::test_statement_update()
{
  connection_.open();

  query<person> q(connection_, "person");

  statement<person> stmt(q.create().prepare());

  result<person> res(stmt.execute());

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto &name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    stmt = q.insert(p).prepare();
    stmt.bind(0, &p);
    stmt.execute();
  }

  column name("name");
  stmt = q.select().where(name == "").prepare();
  matador::varchar<255> hname("hans");
  stmt.bind(0, hname);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> p(first.release());
    UNIT_EXPECT_EQUAL(p->id(), 1UL, "expected id must be 1");
    UNIT_EXPECT_EQUAL(p->name(), "hans", "expected name must be 'hans'");
    UNIT_EXPECT_EQUAL(p->height(), 180U, "expected height must be 180");
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");
    ++first;
  }

//  auto id_cond = id_condition_builder::build<person>();

  matador::column idcol("id");
  person hans("hans", matador::date(15, 6, 1990), 165);
  hans.id(1);

  stmt = q.update(hans).where(idcol == 7).prepare();
  size_t pos = 0;
  pos = stmt.bind(pos, &hans);
  unsigned long hid = 1;
  stmt.bind(pos, hid);

  res = stmt.execute();

  stmt = q.select().where(name == "").prepare();
  stmt.bind(0, hname);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<person> p(first.release());
    UNIT_EXPECT_EQUAL(p->id(), 1UL, "expected id must be 1");
    UNIT_EXPECT_EQUAL(p->name(), "hans", "expected name must be 'hans'");
    UNIT_EXPECT_EQUAL(p->height(), 165U, "expected height must be 180");
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(15, 6, 1990), "expected birthdate is 12.3.1980");
    ++first;
  }

  stmt = q.drop().prepare();

  res = stmt.execute();

}

void QueryTestUnit::test_delete()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  person hans("Hans", matador::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(hans).execute(connection_);

  column name("name");
  res = q.select().where(name == "Hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_EXPECT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_EXPECT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_EXPECT_EQUAL(item->birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  q.remove().where(name == "Hans").execute(connection_);

  res = q.select().where(name == "Hans").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end(), "result list must be empty");

  q.drop().execute(connection_);
}

//void QueryTestUnit::test_foreign_query()
//{
//  connection_.open();
//
//  query<Item> q("item");
//
//  using t_object_item = ObjectItem<Item>;
//
//  // create item table and insert item
//  result<Item> res(q.create().execute(connection_));
//
//  auto itime = time_val_;
//  matador::identifier<unsigned long> id(23);
//  Item *hans = new Item("Hans", 4711);
//  hans->id(id.value());
//  hans->set_time(itime);
//  res = q.insert(*hans).execute(connection_);
//
//  query<t_object_item> object_item_query("object_item");
//  result<t_object_item> ores = object_item_query.create().execute(connection_);
//
//  t_object_item oitem;
//  oitem.ptr(hans);
//
//  ores = object_item_query.insert(oitem).execute(connection_);
//
//  ores = object_item_query.select().execute(connection_);
//
//  auto first = ores.begin();
//  auto last = ores.end();
//
//  while (first != last) {
//    std::unique_ptr<t_object_item> obj(first.release());
//
//    object_ptr<Item> i = obj->ptr();
//    UNIT_ASSERT_TRUE(i.has_primary_key(), "expected valid identifier");
//    UNIT_ASSERT_TRUE(*i.primary_key() == id, "expected id must be 23");
//
//    ++first;
//  }
//
//  object_item_query.drop().execute(connection_);
//
//  q.drop().execute(connection_);
//}

void QueryTestUnit::test_query()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  person hans("Hans", matador::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(hans).execute(connection_);

  column name("name");
  res = q.select().where(name == "hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  res = q.select().where(name == "heinz").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end(), "begin must be equal end");

  q.drop().execute(connection_);
}

void QueryTestUnit::test_query_range_loop()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).execute(connection_);

  column name("name");
  column height("height");
  res = q.select().where(name != "Hans" && (height > 160 && height < 180)).execute(connection_);

  std::vector<std::string> result_names({"Hilde", "Trude"});
  unsigned size(0);
  for (auto &&item : res) {
    ++size;

     UNIT_EXPECT_TRUE(contains(result_names, item->name()), "expected name '" + item->name() + "'not found");
//    UNIT_ASSERT_EQUAL(item.height(), 180U, "expected height must be 180");
//    UNIT_ASSERT_EQUAL(item.birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");
  }

  UNIT_ASSERT_EQUAL(size, 2U, "result size must be two (2)");

  res = q.select().where(name == "heinz").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end(), "begin must be equal end");

  q.drop().execute(connection_);

}

void QueryTestUnit::test_query_select()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).execute(connection_);

  res = q.select().execute(connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 4UL, "result size must be one (4)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());
    ++first;
  }

  column name("name");
  res = q.select().where(name == "Hans").execute(connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "result size must be one (1)");

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  res = q.select().order_by("height").asc().execute(connection_);

  first = res.begin();

  bool is_first = true;
  while (first != last) {
    std::unique_ptr<person> item(first.release());

    if (is_first) {
      UNIT_ASSERT_EQUAL(item->name(), "Otto", "expected name must be 'Otto'");
      UNIT_ASSERT_EQUAL(item->height(), 159U, "expected height must be 159");
      is_first = false;
    }

    ++first;
  }

  column height("height");
  res = q.select()
    .where(height > 160 && height < 180)
    .order_by("height")
    .desc()
    .execute(connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 2UL, "result size must be one (2)");

  first = res.begin();

  is_first = true;
  while (first != last) {
    std::unique_ptr<person> item(first.release());

    if (is_first) {
      UNIT_ASSERT_EQUAL(item->name(), "Hilde", "expected name must be 'Hilde'");
      UNIT_ASSERT_EQUAL(item->height(), 175U, "expected height must be 175");
      is_first = false;
    }

    ++first;
  }

  res = q.drop().execute(connection_);
}

void QueryTestUnit::test_query_select_count()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).execute(connection_);

  query<> count;

  column name("name");
  auto rowres = count.select({columns::count_all()}).from("person").where(name == "Hans" || name == "Hilde").execute(connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(2, item->at<int>(0), "invalid value");
    ++first;
  }

  q.drop().execute(connection_);

}

void QueryTestUnit::test_query_select_columns()
{
  connection_.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).execute(connection_);

  column name("name");

  query<> cols;

  auto rowres = cols.select({"id", "name"}).from("person").where(name == "Hans").execute(connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(1L, item->at<long>("id"), "invalid value");
    UNIT_EXPECT_EQUAL("Hans", item->at<std::string>(name.name), "invalid value");
    ++first;
  }

  q.drop().execute(connection_);
}

struct relation
{
  typedef unsigned long t_id;
  t_id owner = 0UL;
  t_id item = 0UL;

  relation() = default;
  relation(t_id oid, t_id iid)
    : owner(oid), item(iid)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("owner_id", owner);
    serializer.serialize("item_id", item);
  }
};

void QueryTestUnit::test_select_limit()
{
  connection_.open();

  query<relation> q("relation");

  // create item table and insert item
  result<relation> res(q.create().execute(connection_));

  auto r1 = std::make_unique<relation>(1UL, 1UL);
//  std::unique_ptr<relation> r1(new relation(1UL, 1UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 1UL);
//  r1.reset(new relation(1UL, 1UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 2UL);
//  r1.reset(new relation(1UL, 2UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 3UL);
//  r1.reset(new relation(2UL, 3UL));
  res = q.insert(*r1).execute(connection_);

  q.select().limit(1);

  res = q.execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<relation> item(first.release());
    ++first;
  }

  q.drop().execute(connection_);
}

void QueryTestUnit::test_update_limit()
{
  connection_.open();

  query<relation> q("relation");

  // create item table and insert item
  result<relation> res(q.create().execute(connection_));

  auto r1 = std::make_unique<relation>(1UL, 1UL);
//  std::unique_ptr<relation> r1(new relation(1UL, 1UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 1UL);
//  r1.reset(new relation(1UL, 1UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 2UL);
//  r1.reset(new relation(1UL, 2UL));
  res = q.insert(*r1).execute(connection_);
  r1 = std::make_unique<relation>(1UL, 3UL);
//  r1.reset(new relation(2UL, 3UL));
  res = q.insert(*r1).execute(connection_);

  matador::column owner("owner_id");
  matador::column item("item_id");
  relation::t_id newid(4UL);
  q.update({{item.name, newid}}).where(owner == 1 && item == 1).limit(1);

  res = q.execute(connection_);

  q.drop().execute(connection_);
}

void QueryTestUnit::test_prepared_statement()
{
  connection_.open();

  query<> q(connection_, "person");

  q.create({
	  make_typed_id_column<long>("id"),
	  make_typed_varchar_column<32>("name"),
	  make_typed_column<unsigned>("age")
  });

  auto stmt = q.prepare();

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"), "table person must exist");
  auto fields = connection_.describe("person");

  auto cols = { "id", "name", "age" };

  for (const auto &fld : fields) {
	  UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end(), "couldn't find expected field");
  }

  q.drop().execute();
}

void QueryTestUnit::test_prepared_statement_creation()
{
  connection_.open();

  query<person> q(connection_, "person");

  q.create();

  auto stmt = q.prepare();

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"), "table person must exist");

  q.drop().execute();
}

void QueryTestUnit::test_prepared_object_result_twice()
{
  std::cout << "\n";

  connection_.open();

  query<person> q(connection_, "person");

  q.create();

  auto stmt = q.prepare();

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"), "table person must exist");

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto& name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    q.insert(p).execute();
  }

  stmt = q.select().prepare();

  {
    std::set<std::string> nameset;

    for(const auto &name : names) {
      nameset.insert(name);
    }
    auto result = stmt.execute();

    for (auto p : result) {
      auto i = nameset.find(p->name());
      UNIT_EXPECT_TRUE(i != nameset.end(), "name " + p->name() + " not found");
      nameset.erase(i);
    }
  }

  stmt.reset();

  {
    std::set<std::string> nameset;

    for(const auto &name : names) {
      nameset.insert(name);
    }

    auto result = stmt.execute();

    for (auto p : result) {
      auto i = nameset.find(p->name());
      UNIT_EXPECT_TRUE(i != nameset.end(), "name " + p->name() + " not found");
      nameset.erase(i);
    }
  }

  q.drop().execute();
}

void QueryTestUnit::test_prepared_scalar_result_twice()
{
  connection_.open();

  query<> q(connection_, "person");

  q.create({
             make_typed_id_column<long>("id"),
           });

  q.execute();

  std::vector<long> ids({ 1,2,3,4 });

  for (long id : ids) {
    q.insert({"id"}).values({id}).execute();
  }

  auto stmt = q.select({"id"}).from("person").prepare();

  {
    std::set<long> idset;

    for(auto id : ids) {
      idset.insert(id);
    }
    auto result = stmt.execute();

    for (auto p : result) {
      auto i = idset.find(p->at<long>("id"));
      UNIT_EXPECT_TRUE(i != idset.end(), "id " + p->str("id") + " not found");
      idset.erase(i);
    }
  }

  {
    std::set<long> idset;

    for(auto id : ids) {
      idset.insert(id);
    }
    auto result = stmt.execute();

    for (auto p : result) {
      auto i = idset.find(p->at<long>("id"));
      UNIT_EXPECT_TRUE(i != idset.end(), "id " + p->str("id") + " not found");
      idset.erase(i);
    }
  }


  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_rows()
{
  connection_.open();

  query<> q(connection_, "item");

  auto cols = {"id", "string", "varchar", "int", "float", "double", "date", "time"};

  q.create({
             make_typed_id_column<long>("id"),
             make_typed_column<std::string>("string"),
             make_typed_varchar_column<32>("varchar"),
             make_typed_column<int>("int"),
             make_typed_column<float>("float"),
             make_typed_column<double>("double"),
             make_typed_column<matador::date>("date"),
             make_typed_column<matador::time>("time"),
           });

  q.execute();

  UNIT_ASSERT_TRUE(connection_.exists("item"), "table item must exist");
  auto fields = connection_.describe("item");

  for (const auto &fld : fields) {
    UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end(), "couldn't find expected field");
  }

  q
    .insert({"id", "string", "varchar", "int", "float", "double", "date", "time"})
    .values({1, "long text", "good day", -17, 3.1415f, 2.71828, matador::date(), matador::time::now()})
    .execute();

  auto res = q.select({"id", "string", "varchar", "int", "float", "double"}).from("item").execute();

  for (auto item : res) {
    UNIT_EXPECT_EQUAL(1L, item->at<long>("id"), "invalid value");
    UNIT_EXPECT_EQUAL("long text", item->at<std::string>("string"), "invalid value");
    UNIT_EXPECT_EQUAL(-17, item->at<int>("int"), "invalid value");
    UNIT_EXPECT_EQUAL(3.1415f, item->at<float>("float"), "invalid value");
    UNIT_EXPECT_EQUAL(2.71828, item->at<double>("double"), "invalid value");
  }

  q.drop("item").execute();

}

connection QueryTestUnit::create_connection()
{
  return connection(db_);
}

std::string QueryTestUnit::db() const {
  return db_;
}
