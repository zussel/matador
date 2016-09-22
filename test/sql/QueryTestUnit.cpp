//
// Created by sascha on 9/7/15.
//

#include "QueryTestUnit.hpp"

#include "../Item.hpp"

#include "sql/query.hpp"

using namespace oos;

QueryTestUnit::QueryTestUnit(const std::string &name, const std::string &msg, const std::string &db, const oos::time &timeval)
  : unit_test(name, msg)
  , db_(db)
  , time_val_(timeval)
{
  add_test("datatypes", std::bind(&QueryTestUnit::test_datatypes, this), "test sql datatypes");
  add_test("describe", std::bind(&QueryTestUnit::test_describe, this), "test describe table");
  add_test("identifier", std::bind(&QueryTestUnit::test_identifier, this), "test sql identifier");
  add_test("create", std::bind(&QueryTestUnit::test_create, this), "test direct sql create statement");
  add_test("statement_insert", std::bind(&QueryTestUnit::test_statement_insert, this), "test prepared sql insert statement");
  add_test("statement_update", std::bind(&QueryTestUnit::test_statement_update, this), "test prepared sql update statement");
  add_test("foreign_query", std::bind(&QueryTestUnit::test_foreign_query, this), "test query with foreign key");
  add_test("query", std::bind(&QueryTestUnit::test_query, this), "test query");
  add_test("select", std::bind(&QueryTestUnit::test_query_select, this), "test query select");
  add_test("select_count", std::bind(&QueryTestUnit::test_query_select_count, this), "test query select count");
  add_test("select_columns", std::bind(&QueryTestUnit::test_query_select_columns, this), "test query select columns");
  add_test("select_limit", std::bind(&QueryTestUnit::test_select_limit, this), "test query select limit");
  add_test("update_limit", std::bind(&QueryTestUnit::test_update_limit, this), "test query update limit");
}

void QueryTestUnit::initialize()
{
  connection_.reset(create_connection());
}

void QueryTestUnit::test_datatypes()
{
  connection_->open();

  query<Item> q("item");

  q.create().execute(*connection_);

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
  oos::varchar<32> vval("hallo welt");
  std::string strval = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
  oos::date date_val(15, 3, 2015);
  oos::time time_val(time_val_);
  std::unique_ptr<Item> item(new Item);

  // set values
  item->set_bool(bval);
  item->set_char(cval);
  item->set_double(dval);
  item->set_float(fval);
  item->set_short(sval);
  item->set_int(ival);
  item->set_long(lval);
  item->set_unsigned_short(usval);
  item->set_unsigned_int(uival);
  item->set_unsigned_long(ulval);
  item->set_cstr(cstr, strlen(cstr) + 1);
  item->set_varchar(vval);
  item->set_string(strval);
  item->set_date(date_val);
  item->set_time(time_val);

  q.insert(item.get()).execute(*connection_);

  auto res = q.select().execute(*connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last, "first must not be last");

  item.reset((first++).release());

  UNIT_EXPECT_EQUAL(item->get_char(), cval, "character is not equal");
  UNIT_ASSERT_EQUAL(item->get_short(), sval, "short is not equal");
  UNIT_ASSERT_EQUAL(item->get_int(), ival, "integer is not equal");
  UNIT_ASSERT_EQUAL(item->get_long(), lval, "long is not equal");
  UNIT_ASSERT_EQUAL(item->get_unsigned_short(), usval, "unsigned short is not equal");
  UNIT_ASSERT_EQUAL(item->get_unsigned_int(), uival, "unsigned integer is not equal");
  UNIT_ASSERT_EQUAL(item->get_unsigned_long(), ulval, "unsigned long is not equal");
  UNIT_ASSERT_EQUAL(item->get_bool(), bval, "bool is not equal");
  UNIT_ASSERT_EQUAL(item->get_cstr(), cstr, "const char pointer is not equal");
  UNIT_ASSERT_EQUAL(item->get_string(), strval, "strings is not equal");
  UNIT_ASSERT_EQUAL(item->get_varchar(), vval, "varchar is not equal");
  UNIT_ASSERT_EQUAL(item->get_date(), date_val, "date is not equal");
  UNIT_ASSERT_EQUAL(item->get_time(), time_val, "time is not equal");

  q.drop().execute(*connection_);
}

void QueryTestUnit::test_describe()
{
  connection_->open();

  oos::query<person> q("person");

  q.create().execute(*connection_);

  auto fields = connection_->describe("person");

  std::vector<std::string> columns = { "id", "name", "birthdate", "height"};
  std::vector<data_type_t > types = { oos::type_long, oos::type_varchar, oos::type_text, oos::type_long};

  for (auto &&field : fields) {
    UNIT_ASSERT_EQUAL(field.name(), columns[field.index()], "invalid column name");
    UNIT_ASSERT_EQUAL(field.type(), types[field.index()], "invalid column type");
//    std::cout << "\n" << field.index() << " column: " << field.name() << " (type: " << field.type() << ")";
  }

  q.drop().execute(*connection_);
}

class pktest
{
public:
  pktest() {}
  pktest(unsigned long i, const std::string &n)
    : id(i), name(n)
  {}

  ~pktest() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
  }

  oos::identifier<unsigned long> id;
  std::string name;
};

void QueryTestUnit::test_identifier()
{
  connection_->open();

  oos::query<pktest> q("pktest");

  q.create().execute(*connection_);

  std::unique_ptr<pktest> p(new pktest(7, "hans"));

  UNIT_EXPECT_EQUAL(p->id.value(), 7UL, "identifier value should be greater zero");

  q.insert(p.get()).execute(*connection_);

  auto res = q.select().execute(*connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last, "first must not be last");

  p.reset((first++).release());

  UNIT_EXPECT_GREATER(p->id.value(), 0UL, "identifier value should be greater zero");

  q.drop().execute(*connection_);
}

void QueryTestUnit::test_create()
{
  connection_->open();

  query<Item> q("item");

  result<Item> res(q.create().execute(*connection_));

  auto itime = time_val_;
  Item hans("Hans", 4711);
  hans.set_time(itime);
  res = q.insert(&hans).execute(*connection_);

  res = q.select().execute(*connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  res = q.drop().execute(*connection_);
}

void QueryTestUnit::test_statement_insert()
{
  connection_->open();

  query<Item> q("item");

  statement<Item> stmt(q.create().prepare(*connection_));

  result<Item> res(stmt.execute());

  oos::identifier<unsigned long> id(23);
  auto itime = time_val_;
  Item hans("Hans", 4711);
  hans.id(id.value());
  hans.set_time(itime);
  stmt = q.insert(&hans).prepare(*connection_);

  stmt.bind(&hans, 0);
  res = stmt.execute();

  stmt = q.select().prepare(*connection_);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->id(), 23UL, "expected id must be 23");
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  stmt = q.drop().prepare(*connection_);

  res = stmt.execute();
}

void QueryTestUnit::test_statement_update()
{
  connection_->open();

  query<Item> q("item");

  statement<Item> stmt(q.create().prepare(*connection_));

  result<Item> res(stmt.execute());

  oos::identifier<unsigned long> id(23);
  auto itime = time_val_;
  Item hans("Hans", 4711);
  hans.id(id.value());
  hans.set_time(itime);
  stmt = q.insert(&hans).prepare(*connection_);

  stmt.bind(&hans, 0);
  res = stmt.execute();

  stmt = q.select().prepare(*connection_);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->id(), 23UL, "expected id must be 23");
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

//  auto id_cond = id_condition_builder::build<person>();

  oos::column idcol("id");
  int i815 = 815;
  stmt = q.update().set("val_int", i815).where(idcol == 7).prepare(*connection_);
  size_t pos = 0;
  pos = stmt.bind(i815, pos);
  unsigned long hid = hans.id();
  stmt.bind(hid, pos);

  res = stmt.execute();

  stmt = q.select().prepare(*connection_);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->id(), 23UL, "expected id must be 23");
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 815, "expected integer must be 815");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  hans.set_int(4711);
  stmt = q.update().set(&hans).where(idcol == 7).prepare(*connection_);
  pos = 0;
  pos = stmt.bind(&hans, pos);
  stmt.bind(hid, pos);

  res = stmt.execute();

  stmt = q.select().prepare(*connection_);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "expected size must be one (1)");

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->id(), 23UL, "expected id must be 23");
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  stmt = q.drop().prepare(*connection_);

  res = stmt.execute();

}

void QueryTestUnit::test_foreign_query()
{
  connection_->open();

  query<Item> q("item");

  using t_object_item = ObjectItem<Item>;

  // create item table and insert item
  result<Item> res(q.create().execute(*connection_));

  auto itime = time_val_;
  oos::identifier<unsigned long> id(23);
  Item *hans = new Item("Hans", 4711);
  hans->id(id.value());
  hans->set_time(itime);
  res = q.insert(hans).execute(*connection_);

  query<t_object_item> object_item_query("object_item");
  result<t_object_item> ores = object_item_query.create().execute(*connection_);

  t_object_item oitem;
  oitem.ptr(hans);

  ores = object_item_query.insert(&oitem).execute(*connection_);

  ores = object_item_query.select().execute(*connection_);

  auto first = ores.begin();
  auto last = ores.end();

  while (first != last) {
    std::unique_ptr<t_object_item> obj(first.release());

    object_ptr<Item> i = obj->ptr();
    UNIT_ASSERT_TRUE(i.has_primary_key(), "expected valid identifier");
    UNIT_ASSERT_TRUE(*i.primary_key() == id, "expected id must be 23");

    ++first;
  }

  object_item_query.drop().execute(*connection_);

  q.drop().execute(*connection_);
}

void QueryTestUnit::test_query()
{
  connection_->open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(*connection_));

  person hans("Hans", oos::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(&hans).execute(*connection_);

  column name("name");
  res = q.select().where(name == "hans").execute(*connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), oos::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  res = q.select().where(name == "heinz").execute(*connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end(), "begin must be equal end");

  q.drop().execute(*connection_);
}

void QueryTestUnit::test_query_select()
{
  connection_->open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(*connection_));

  unsigned long counter = 0;

  std::unique_ptr<person> hans(new person(++counter, "Hans", oos::date(12, 3, 1980), 180));
  res = q.insert(hans.get()).execute(*connection_);

  std::unique_ptr<person> otto(new person(++counter, "Otto", oos::date(27, 11, 1954), 159));
  res = q.insert(otto.get()).execute(*connection_);

  std::unique_ptr<person> hilde(new person(++counter, "Hilde", oos::date(13, 4, 1975), 175));
  res = q.insert(hilde.get()).execute(*connection_);

  std::unique_ptr<person> trude(new person(++counter, "Trude", oos::date(1, 9, 1967), 166));
  res = q.insert(trude.get()).execute(*connection_);

  res = q.select().execute(*connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 4UL, "result size must be one (4)");

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());
    ++first;
  }

  column name("name");
  res = q.select().where(name == "Hans").execute(*connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 1UL, "result size must be one (1)");

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), oos::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  res = q.select().order_by("height").asc().execute(*connection_);

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
    .execute(*connection_);

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

  res = q.drop().execute(*connection_);
}

void QueryTestUnit::test_query_select_count()
{
  connection_->open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(*connection_));

  unsigned long counter = 0;

  std::unique_ptr<person> hans(new person(++counter, "Hans", oos::date(12, 3, 1980), 180));
  res = q.insert(hans.get()).execute(*connection_);

  std::unique_ptr<person> otto(new person(++counter, "Otto", oos::date(27, 11, 1954), 159));
  res = q.insert(otto.get()).execute(*connection_);

  std::unique_ptr<person> hilde(new person(++counter, "Hilde", oos::date(13, 4, 1975), 175));
  res = q.insert(hilde.get()).execute(*connection_);

  std::unique_ptr<person> trude(new person(++counter, "Trude", oos::date(1, 9, 1967), 166));
  res = q.insert(trude.get()).execute(*connection_);

  query<> count;

  column name("name");
  auto rowres = count.select({columns::count_all()}).from("person").where(name == "Hans" || name == "Hilde").execute(*connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(2, item->at<int>(0), "invalid value");
    ++first;
  }

  q.drop().execute(*connection_);

}

void QueryTestUnit::test_query_select_columns()
{
  connection_->open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(*connection_));

  unsigned long counter = 0;

  std::unique_ptr<person> hans(new person(++counter, "Hans", oos::date(12, 3, 1980), 180));
  res = q.insert(hans.get()).execute(*connection_);

  std::unique_ptr<person> otto(new person(++counter, "Otto", oos::date(27, 11, 1954), 159));
  res = q.insert(otto.get()).execute(*connection_);

  std::unique_ptr<person> hilde(new person(++counter, "Hilde", oos::date(13, 4, 1975), 175));
  res = q.insert(hilde.get()).execute(*connection_);

  std::unique_ptr<person> trude(new person(++counter, "Trude", oos::date(1, 9, 1967), 166));
  res = q.insert(trude.get()).execute(*connection_);

  column id("id");
  column name("name");

  query<> cols;

  auto rowres = cols.select({id, name}).from("person").where(name == "Hans").execute(*connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(1L, item->at<long>(id.name), "invalid value");
    UNIT_EXPECT_EQUAL("Hans", item->at<std::string>(name.name), "invalid value");
//    std::cout << "id " << item->str(id.name) << ", name " << item->at<std::string>(name.name) << "\n";
    ++first;
  }

  q.drop().execute(*connection_);
}

struct relation
{
  typedef unsigned long t_id;
  t_id owner = 0UL;
  t_id item = 0UL;

  relation() {}
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
  connection_->open();

  query<relation> q("relation");

  // create item table and insert item
  result<relation> res(q.create().execute(*connection_));

  std::unique_ptr<relation> r1(new relation(1UL, 1UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(1UL, 1UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(1UL, 2UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(2UL, 3UL));
  res = q.insert(r1.get()).execute(*connection_);

  q.select().limit(1);

  res = q.execute(*connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<relation> item(first.release());
    ++first;
  }

  q.drop().execute(*connection_);

  connection_->close();
}

void QueryTestUnit::test_update_limit()
{
  connection_->open();

  query<relation> q("relation");

  // create item table and insert item
  result<relation> res(q.create().execute(*connection_));

  std::unique_ptr<relation> r1(new relation(1UL, 1UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(1UL, 1UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(1UL, 2UL));
  res = q.insert(r1.get()).execute(*connection_);
  r1.reset(new relation(2UL, 3UL));
  res = q.insert(r1.get()).execute(*connection_);

  oos::column owner("owner_id");
  oos::column item("item_id");
  relation::t_id newid(4UL);
  q.update().set(item.name, newid).where(owner == 1 && item == 1).limit(1);

  res = q.execute(*connection_);

  q.drop().execute(*connection_);

  connection_->close();
}

connection* QueryTestUnit::create_connection()
{
  return new connection(db_);
}

std::string QueryTestUnit::db() const {
  return db_;
}
