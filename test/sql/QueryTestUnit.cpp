#include "QueryTestUnit.hpp"

#include "../datatypes.hpp"
#include "../person.hpp"
#include "../entities.hpp"

#include "matador/sql/query.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/types.hpp"
#include "matador/sql/database_error.hpp"
#include "matador/sql/schema.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <algorithm>
#include <limits>
#include <set>
#include <iomanip>

using namespace matador;

QueryTestUnit::QueryTestUnit(const std::string &prefix, std::string db, matador::time timeval)
  : unit_test(prefix + "_query", prefix + " query test unit")
  , db_(std::move(db))
  , db_vendor_(prefix)
  , time_val_(timeval)
{
  add_test("info", [this] { print_data_types(); }, "print datatypes info");
  add_test("datatypes", [this] { test_data_types(); }, "test sql datatypes");
  add_test("schema", [this] { test_schema(); }, "test schema");
  add_test("qvc", [this] { test_query_value_creator(); }, "test query value creator");
  add_test("quoted_identifier", [this] { test_quoted_identifier(); }, "test quoted identifier");
  add_test("columns_with_quotes", [this] { test_columns_with_quotes_in_name(); }, "test columns with quotes in name");
  add_test("quoted_literals", [this] { test_quoted_literals(); }, "test quoted literals");
//  add_test("bind_tablename", [this] { test_bind_tablename(); }, "test bind tablenames");
  add_test("describe", [this] { test_describe(); }, "test describe table");
  add_test("unknown_table", [this] { test_unknown_table(); }, "test unknown table");
  add_test("identifier", [this] { test_identifier(); }, "test sql identifier");
  add_test("identifier_prepared", [this] { test_identifier_prepared(); }, "test sql prepared identifier");
  add_test("select_time", [this] { test_select_time(); }, "test select time value");
  add_test("update", [this] { test_update(); }, "test direct sql update statement");
  add_test("create", [this] { test_create(); }, "test direct sql create statement");
  add_test("create_anonymous", [this] { test_anonymous_create(); }, "test direct sql create statement via row (anonymous)");
  add_test("insert_anonymous", [this] { test_anonymous_insert(); }, "test direct sql insert statement via row (anonymous)");
  add_test("update_anonymous", [this] { test_anonymous_update(); }, "test direct sql update statement via row (anonymous)");
  add_test("statement_insert", [this] { test_statement_insert(); }, "test prepared sql insert statement");
  add_test("statement_update", [this] { test_statement_update(); }, "test prepared sql update statement");
  add_test("statement_delete", [this] { test_statement_delete(); }, "test prepared sql delete statement");
  add_test("delete", [this] { test_delete(); }, "test query delete");
  add_test("multiple_delete", [this] { test_multiple_delete(); }, "test query multiple delete");
//  add_test("foreign_query", std::bind(&QueryTestUnit::test_foreign_query, this), "test query with foreign key");
  add_test("query", [this] { test_query(); }, "test query");
  add_test("result_range", [this] { test_query_range_loop(); }, "test result range loop");
  add_test("select", [this] { test_query_select(); }, "test query select");
  add_test("select_count", [this] { test_query_select_count(); }, "test query select count");
  add_test("select_columns", [this] { test_query_select_columns(); }, "test query select columns");
  add_test("select_like", [this] { test_query_select_like(); }, "test query select like");
  add_test("select_limit", [this] { test_select_limit(); }, "test query select limit");
  add_test("update_limit", [this] { test_update_limit(); }, "test query update limit");
  add_test("prepared_statement", [this] { test_prepared_statement(); }, "test query prepared statement");
  add_test("prepared_statement_creation", [this] { test_prepared_statement_creation(); }, "test query prepared statement creation");
  add_test("object_result_twice", [this] { test_prepared_object_result_twice(); }, "test query prepared statement get object result twice");
  add_test("scalar_result_twice", [this] { test_prepared_scalar_result_twice(); }, "test query prepared statement get scalar result twice");
  add_test("rows", [this] { test_rows(); }, "test row value serialization");
  add_test("log", [this] { test_log(); }, "test log behavior");
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

using namespace matador;

void QueryTestUnit::print_data_types()
{
  std::cout << "\n";
  std::cout << std::setw(20) << "type" << "|" << std::setw(8) << "size" << "\n";
  std::cout << std::setw(20) << std::setfill('=') << "=" << "+" << std::setw(8) <<"=" << std::setfill(' ') << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<char>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<char>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<short>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<short>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<int>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<int>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<long>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<long>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<long long>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<long long>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<unsigned char>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<unsigned char>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<unsigned short>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<unsigned short>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<unsigned int>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<unsigned int>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<unsigned long>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<unsigned long>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<unsigned long long>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<unsigned long long>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<bool>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<bool>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<float>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<float>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<double>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<double>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<std::string>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<std::string>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<const char*>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<const char*>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<matador::date>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<matador::date>::size() << "\n";
  std::cout << std::left << std::setw(20) << data_type_traits<matador::time>::name() << "|" <<  std::right <<  std::setw(8) << data_type_traits<matador::time>::size() << "\n";
}

void QueryTestUnit::test_data_types()
{
  connection_.connect();

  query<datatypes> q{};

  q.create("datatypes").execute(connection_);

  float fval = 2.445566f;
  double dval = 11111.23433345;
  char cval = 'c';
  short sval = (std::numeric_limits<short>::min)();
  int ival = (std::numeric_limits<int>::min)();
  long lval = (std::numeric_limits<long>::min)();
  long long llval = (std::numeric_limits<long long>::max)();
  unsigned char ucval = (std::numeric_limits<unsigned char>::max)();
  unsigned short usval = (std::numeric_limits<unsigned short>::max)();
  unsigned int uival = (std::numeric_limits<unsigned int>::max)();
  unsigned long ulval = (std::numeric_limits<unsigned long>::max)();
  unsigned long long ullval = (std::numeric_limits<unsigned long long>::max)();
  if (connection_.type() == "sqlite" || connection_.type() == "postgresql") {
    ulval = (std::numeric_limits<long>::max)();
    ullval = (std::numeric_limits<long long>::max)();
  }
  bool bval = true;
  const char *cstr("Armer schwarzer Kater");
  std::string varcharval("hallo welt");
  std::string strval = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam "
                       "nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, "
                       "sed diam voluptua. At vero eos et accusam et justo duo dolores et ea "
                       "rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. "
                       "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy "
                       "eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. "
                       "At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd "
                       "gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
  matador::date date_val(15, 3, 2015);
  matador::time time_val(time_val_);
  datatypes item;

  // set values
  item.set_bool(bval);
  item.set_char(cval);
  item.set_double(dval);
  item.set_float(fval);
  item.set_short(sval);
  item.set_int(ival);
  item.set_long(lval);
  item.set_long_long(llval);
  item.set_unsigned_char(ucval);
  item.set_unsigned_short(usval);
  item.set_unsigned_int(uival);
  item.set_unsigned_long(ulval);
  item.set_unsigned_long_long(ullval);
  item.set_cstr(cstr, strlen(cstr) + 1);
  item.set_varchar(varcharval);
  item.set_string(strval);
  item.set_date(date_val);
  item.set_time(time_val);

  q.insert(item).into("datatypes").values(item).execute(connection_);

  auto res = q.select().from("datatypes").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last);

  std::unique_ptr<datatypes> it((first++).release());

  UNIT_EXPECT_EQUAL(it->get_char(), cval);
  UNIT_EXPECT_EQUAL(it->get_short(), sval);
  UNIT_EXPECT_EQUAL(it->get_int(), ival);
  UNIT_EXPECT_EQUAL(it->get_long(), lval);
  UNIT_EXPECT_EQUAL(it->get_long_long(), llval);
  UNIT_EXPECT_EQUAL(it->get_unsigned_char(), ucval);
  UNIT_EXPECT_EQUAL(it->get_unsigned_short(), usval);
  UNIT_EXPECT_EQUAL(it->get_unsigned_int(), uival);
  UNIT_EXPECT_EQUAL(it->get_unsigned_long(), ulval);
  UNIT_EXPECT_EQUAL(it->get_unsigned_long_long(), ullval);
  UNIT_EXPECT_EQUAL(it->get_bool(), bval);
  UNIT_EXPECT_EQUAL(it->get_cstr(), cstr);
  UNIT_EXPECT_EQUAL(it->get_string(), strval);
  UNIT_EXPECT_EQUAL(it->get_varchar(), varcharval);
  UNIT_EXPECT_EQUAL(it->get_date(), date_val);
  UNIT_EXPECT_EQUAL(it->get_time(), time_val);

  res.close();
  q.drop("datatypes").execute(connection_);
}

void QueryTestUnit::test_schema()
{
  matador::schema s;

  s.attach<person>("person");

  auto info = s.find<person>();

  UNIT_ASSERT_NOT_NULL(info.get());
  UNIT_ASSERT_EQUAL(info->name, "person");
  UNIT_ASSERT_NULL(info->parent_info.get());

  const std::type_index ti(typeid(person));
  UNIT_ASSERT_TRUE(info->type == ti);
}

void QueryTestUnit::test_query_value_creator()
{
  matador::detail::query_value_creator qvc;

  matador::any ac = 'c';

  auto val = qvc.create_from_any(ac);

  UNIT_ASSERT_EQUAL(val->get<char>(), 'c');
}

void QueryTestUnit::test_quoted_identifier()
{
  connection_.connect();

  query<> q{};

  q.create("quotes", {
      make_column<std::string>("from", 255),
      make_column<std::string>("to", 255)
    })
    .execute(connection_);

  // check table description
  std::vector<std::string> columns = { "from", "to"};
  std::vector<database_type > types = {matador::database_type::type_varchar, matador::database_type::type_varchar};
  auto fields = connection_.describe("quotes");

  for (auto &&field : fields) {
    UNIT_EXPECT_EQUAL(field.name(), columns[field.index()]);
    UNIT_EXPECT_EQUAL((int)field.type(), (int)types[field.index()]);
  }

  q.insert({"from", "to"})
   .into("quotes")
   .values({"Berlin", "London"})
   .execute(connection_);

  auto res = q.select({"from", "to"})
              .from("quotes")
              .execute(connection_);

  for (auto row : res) {
    UNIT_EXPECT_EQUAL("Berlin", row->at<std::string>("from"));
    UNIT_EXPECT_EQUAL("London", row->at<std::string>("to"));
  }

  column from("from");
  q.update("quotes", {{"from", "Hamburg"}, {"to", "New York"}}).where(from == "Berlin").execute(connection_);

  res = q.select({"from", "to"}).from("quotes").execute(connection_);

  for (auto row : res) {
    UNIT_EXPECT_EQUAL("Hamburg", row->at<std::string>("from"));
    UNIT_EXPECT_EQUAL("New York", row->at<std::string>("to"));
  }

  q.drop("from").execute(connection_);
}

void QueryTestUnit::test_columns_with_quotes_in_name()
{
  connection_.connect();

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
    query<> q{};

    q.create("quotes", {make_column<std::string>(colname, 255)})
     .execute(connection_);

    // check table description
    std::vector<std::string> columns({ colname });
    std::vector<database_type > types({matador::database_type::type_varchar });
    auto fields = connection_.describe("quotes");

    for (auto &&field : fields) {
      UNIT_EXPECT_EQUAL(field.name(), columns[field.index()]);
      UNIT_EXPECT_EQUAL((int)field.type(), (int)types[field.index()]);
    }

    q.drop("quotes").execute(connection_);
  }
}

void QueryTestUnit::test_quoted_literals()
{
  connection_.connect();

  query<> q{};

  q.create("escapes", {make_column<std::string>("name", 255)}).execute(connection_);

  q.insert({"name"}).values({"text"}).execute(connection_);

  auto res = q.select({"name"}).from("escapes").execute(connection_);

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text");
  }

  q.update("escapes", {{"name", "text'd"}}).execute(connection_);

  res = q.select({"name"}).from("escapes").execute(connection_);

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text'd");
  }

  q.update("escapes", {{"name", "text\nhello\tworld"}}).execute(connection_);

  res = q.select({"name"}).from("escapes").execute(connection_);

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text\nhello\tworld");
  }

  q.update("escapes", {{"name", "text \"text\""}}).execute(connection_);

  res = q.select({"name"}).from("escapes").execute(connection_);

  for (auto item : res) {
    UNIT_ASSERT_EQUAL(item->at<std::string>(0), "text \"text\"");
  }

  q.drop("escapes").execute(connection_);
}

//void QueryTestUnit::test_bind_tablename()
//{
//  matador::query<person>::clear_bound_tables();
//
//  matador::query<person> q0; // table name should be person
//
//  UNIT_ASSERT_TRUE(q0.table_name().empty());
//  matador::query<person>::bind_table("person");
//
//  matador::query<person> q1; // table name should be person
//
//  UNIT_ASSERT_EQUAL(q1.table_name(), "person");
//
//  matador::query<person> q2("student"); // table name should be student
//
//  UNIT_ASSERT_EQUAL(q2.table_name(), "student");
//}

void QueryTestUnit::test_describe()
{
  connection_.connect();

  matador::query<person> q{};

  q.create("person").execute(connection_);

  auto fields = connection_.describe("person");

  std::vector<std::string> columns = { "id", "name", "birthdate", "height"};
  std::vector<database_type > types = { matador::database_type::type_bigint, matador::database_type::type_varchar, matador::database_type::type_date, matador::database_type::type_bigint};

  for (auto &&field : fields) {
    UNIT_ASSERT_EQUAL(field.name(), columns[field.index()]);
    UNIT_ASSERT_EQUAL((int)field.type(), (int)types[field.index()]);
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_unknown_table()
{
  connection_.connect();

  matador::query<person> q{};

  bool caught_exception = false;

  try {
    q.select().from("person").execute(connection_);
  } catch (database_error &ex) {
    caught_exception = true;
    if (db_vendor_ == "postgresql") {
      UNIT_EXPECT_EQUAL("42P01", ex.sql_state());
    } else if (db_vendor_ == "mysql") {
      UNIT_EXPECT_EQUAL("42S02", ex.sql_state());
    } else if (db_vendor_ == "mssql") {
      UNIT_EXPECT_EQUAL("42S02", ex.sql_state());
    } else if (db_vendor_ == "sqlite") {
      UNIT_EXPECT_EQUAL(1L, ex.error_code());
    } else {
      UNIT_FAIL("invalid database vendor string: " << db_vendor_);
    }
  } catch (...) {
    UNIT_FAIL("caught from exception");
  }
  UNIT_ASSERT_TRUE(caught_exception);
}

class pktest
{
public:
  pktest() = default;
  pktest(unsigned long i, std::string n)
    : id(i), name(std::move(std::move(n)))
  {}

  ~pktest() = default;

  template < class Operator >
  void process(Operator &op)
  {
      matador::access::primary_key(op, "id", id);
      matador::access::attribute(op, "name", name, 255);
  }

  unsigned long id{};
  std::string name;
};

void QueryTestUnit::test_identifier()
{
  connection_.connect();

  matador::query<pktest> q{};

  q.create("pktest").execute(connection_);

  std::unique_ptr<pktest> p(new pktest(7, "hans"));

  UNIT_EXPECT_EQUAL(p->id, 7UL);

  q.insert(*p).into("pktest").execute(connection_);

  auto res = q.select().execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  UNIT_ASSERT_TRUE(first != last);

  p.reset((first++).release());

  UNIT_EXPECT_GREATER(p->id, 0UL);

  q.drop("pktest").execute(connection_);
}

void QueryTestUnit::test_identifier_prepared()
{
  connection_.connect();

  matador::query<pktest> q{};

  auto stmt = q.create("pktest").prepare(connection_);
  stmt.execute();

  pktest p(7, "hans");

  UNIT_EXPECT_EQUAL(p.id, 7UL);

  stmt = q.insert(p).prepare(connection_);
  stmt.bind(0, &p);
  stmt.execute();

  stmt = q.select().from("pktest").prepare(connection_);

  auto res = stmt.execute();

  for (const auto &pres : res) {
    UNIT_EXPECT_EQUAL(pres->name, "hans");
    UNIT_EXPECT_EQUAL(pres->id, 7UL);
  }

  stmt = q.drop("pktest").prepare(connection_);

  stmt.execute();
}

struct appointment
{
  unsigned long id{};
  std::string name;
  matador::time time_point;

  template < class Operator >
  void process(Operator &op)
  {
      matador::access::primary_key(op, "id", id);
      matador::access::attribute(op, "name", name, 255);
      matador::access::attribute(op, "time_point", time_point);
  }
};

void QueryTestUnit::test_select_time()
{
  connection_.connect();

  query<appointment> q{};

  q.create("appointment").execute(connection_);

  auto dinner = appointment{ 1, "dinner", time_val_ };
  auto str = to_string(time_val_);
  q.insert(dinner).into("appointment").execute(connection_);

  auto res = q.select().from("appointment").execute(connection_);

  for (const auto &item : res) {
    UNIT_EXPECT_EQUAL(item->time_point, time_val_);
  }

  q.drop("appointment").execute(connection_);
}

void QueryTestUnit::test_create()
{
  connection_.connect();

  query<datatypes> q{};

  q.create("item").execute(connection_);

  auto itime = time_val_;
  datatypes hans("Hans", 4711);
  hans.set_time(itime);
  result<datatypes> res = q.insert(hans).into("item").execute(connection_);

  res = q.select().from("item").execute(connection_);

//  UNIT_ASSERT_EQUAL(res.size(), 1UL);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<datatypes> item(first.release());
    UNIT_EXPECT_EQUAL(item->get_string(), "Hans");
    UNIT_EXPECT_EQUAL(item->get_int(), 4711);
    UNIT_EXPECT_EQUAL(item->get_time(), itime);
    ++first;
  }

  res = q.drop("item").execute(connection_);
}

void QueryTestUnit::test_update()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto &name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    q.insert(p).execute(connection_);
  }

  res = q.select().from("person").where("name"_col == "hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "hans");
    UNIT_ASSERT_EQUAL(item->height(), 180U);
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980));

    ++first;
  }

  person hans("hans", matador::date(15, 6, 1990), 165);
  hans.id(1);
  column idcol("id");
  q.update("person", hans).where(idcol == 1).execute(connection_);

  res = q.select().from("person").where("name"_col == "hans").execute(connection_);

  for (auto i : res) {
    UNIT_ASSERT_EQUAL(i->name(), "hans");
    UNIT_ASSERT_EQUAL(i->height(), 165U);
    UNIT_ASSERT_EQUAL(i->birthdate(), matador::date(15, 6, 1990));
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_anonymous_create()
{
  connection_.connect();

  query<> q{};

  auto cols = {"id", "name", "age"};

  q.create("person", {
    make_pk_column<long>("id"),
    make_column<std::string>("name", 63),
    make_column<unsigned>("age")
  });

  q.execute(connection_);

  UNIT_ASSERT_TRUE(connection_.exists("person"));
  auto fields = connection_.describe("person");

  for (const auto &fld : fields) {
    UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end());
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_anonymous_insert()
{
  connection_.connect();

  auto q = query<>{};

  q.create("person", {
           make_pk_column<int>("id"),
           make_column<std::string>("name", 63),
           make_column<unsigned>("age")
   });

  q.execute(connection_);

  q.insert({"id", "name", "age"})
   .into("person")
   .values({1, "hans", 45})
   .execute(connection_);

  auto res = q.select({"id", "name", "age"}).from("person").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(1, item->at<int>("id"));
    UNIT_EXPECT_EQUAL("hans", item->at<std::string>("name"));
    UNIT_EXPECT_EQUAL(45U, item->at<unsigned>("age"));
    ++first;
  }

  q.drop("person").execute(connection_);

}

void QueryTestUnit::test_anonymous_update()
{
  connection_.connect();

  query<> q{};

  q.create("person", {
           make_pk_column<long>("id"),
           make_column<std::string>("name", 63),
           make_column<unsigned>("age")
   });

  q.execute(connection_);

  q
   .insert({"id", "name", "age"})
   .values({1, "hans", 45})
   .execute(connection_);

  column name("name");

  q.update("person", {{"name", "jane"}, {"age", 47}})
   .where(name == "hans")
   .execute(connection_);

  auto res = q.select({"id", "name", "age"})
                         .from("person")
                         .execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL("jane", item->at<std::string>("name"));
    UNIT_EXPECT_EQUAL(47U, item->at<unsigned>("age"));
    ++first;
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_statement_insert()
{
  connection_.connect();

  query<datatypes> q{};

  statement<datatypes> stmt(q.create("datatypes").prepare(connection_));

  result<datatypes> res(stmt.execute());

  unsigned long id(23);
  auto itime = time_val_;
  datatypes hans("Hans", 4711);
  hans.id(id);
  hans.set_time(itime);
  stmt = q.insert(hans).into("datatypes").prepare(connection_);

  stmt.bind(0, &hans);

  res = stmt.execute();

  stmt = q.select().from("datatypes").prepare(connection_);

  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL);

  for (auto const &item : res) {
    UNIT_EXPECT_EQUAL(item->id(), 23UL);
    UNIT_EXPECT_EQUAL(item->get_string(), "Hans");
    UNIT_EXPECT_EQUAL(item->get_varchar(), "Erde");
    UNIT_EXPECT_EQUAL(item->get_cstr(), "Hallo");
    UNIT_EXPECT_EQUAL(item->get_int(), 4711);
    UNIT_EXPECT_EQUAL(item->get_unsigned_char(), 'H');
    UNIT_EXPECT_EQUAL(item->get_time(), itime);
  }

  stmt = q.drop("datatypes").prepare(connection_);

  res = stmt.execute();
}

void QueryTestUnit::test_statement_update()
{
  connection_.connect();

  query<person> q{};

  statement<person> stmt(q.create("person").prepare(connection_));

  result<person> res(stmt.execute());

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto &name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    stmt = q.insert(p).into("person").prepare(connection_);
    stmt.bind(0, &p);
    stmt.execute();
  }

  stmt = q.select()
          .from("person")
          .where("name"_col == "")
          .prepare(connection_);
  std::string name("hans");
  stmt.bind(0, name);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL);

  //auto first = res.begin();
  //auto last = res.end();

  for (auto const &p : res) {
    UNIT_EXPECT_EQUAL(p->id(), 1UL);
    UNIT_EXPECT_EQUAL(p->name(), "hans");
    UNIT_EXPECT_EQUAL(p->height(), 180U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(12, 3, 1980));
  }

  matador::column idcol("id");
  person hans("hans", matador::date(15, 6, 1990), 165);
  hans.id(1);

  stmt = q.update("person", hans).where(idcol == 7).prepare(connection_);
  size_t pos = 0;
  pos = stmt.bind(pos, &hans);
  unsigned long hid = 1;
  stmt.bind(pos, hid);

  res = stmt.execute();

  stmt = q.select()
          .from("person")
          .where("name"_col == "")
          .prepare(connection_);
  stmt.bind(0, name);
  res = stmt.execute();

//  UNIT_ASSERT_EQUAL(res.size(), 1UL);

  //first = res.begin();
  //last = res.end();

  for (const auto &p : res) {
    UNIT_EXPECT_EQUAL(p->id(), 1UL);
    UNIT_EXPECT_EQUAL(p->name(), "hans");
    UNIT_EXPECT_EQUAL(p->height(), 165U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(15, 6, 1990));
  }

  stmt = q.drop("person").prepare(connection_);

  res = stmt.execute();

}

void QueryTestUnit::test_statement_delete()
{
  connection_.connect();

  query<person> q{};

  statement<person> stmt(q.create("person").prepare(connection_));

  result<person> res(stmt.execute());

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto &name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    stmt = q.insert(p).into("person").prepare(connection_);
    stmt.bind(0, &p);
    stmt.execute();
  }

  stmt = q.select().from("person").where("name"_col == "").prepare(connection_);
  std::string name("hans");
  stmt.bind(0, name);
  res = stmt.execute();

  for (auto const &p : res) {
    UNIT_EXPECT_EQUAL(p->id(), 1UL);
    UNIT_EXPECT_EQUAL(p->name(), "hans");
    UNIT_EXPECT_EQUAL(p->height(), 180U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(12, 3, 1980));
  }

  column idcol = "name"_col;
  stmt = q.remove("person").where(idcol == "").prepare(connection_);

  std::string hans {"hans"};
  stmt.bind(0, hans);
  res = stmt.execute();

  stmt.reset();

  std::string georg {"georg"};
  stmt.bind(0, georg);
  res = stmt.execute();

  stmt = q.drop("person").prepare(connection_);

  res = stmt.execute();
}

void QueryTestUnit::test_delete()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  person hans("Hans", matador::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(hans).into("person").execute(connection_);

  column name("name");
  res = q.select().where(name == "Hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  for (auto p : res) {
    //while (first != last) {
    //std::unique_ptr<person> item(first.release());

    UNIT_EXPECT_EQUAL(p->name(), "Hans");
    UNIT_EXPECT_EQUAL(p->height(), 180U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(12, 3, 1980));

    //++first;
  }

  q.remove("person").where(name == "Hans").execute(connection_);

  res = q.select().from("person").where(name == "Hans").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end());

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_multiple_delete()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  person hans("Hans", matador::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(hans).into("person").execute(connection_);

  person george("George", matador::date(11, 7, 1979), 198);
  george.id(2);
  res = q.insert(george).into("person").execute(connection_);

  column name("name");
  res = q.select().from("person").where(name == "Hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  for (auto p : res) {
    UNIT_EXPECT_EQUAL(p->name(), "Hans");
    UNIT_EXPECT_EQUAL(p->height(), 180U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(12, 3, 1980));
  }

  q.remove("person").where(name == "Hans").execute(connection_);

  res = q.select().from("person").where(name == "Hans").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end());

  res = q.select().from("person").where(name == "George").execute(connection_);

  for (auto p : res) {
    UNIT_EXPECT_EQUAL(p->name(), "George");
    UNIT_EXPECT_EQUAL(p->height(), 198U);
    UNIT_EXPECT_EQUAL(p->birthdate(), matador::date(11, 7, 1979));
  }

  q.remove("person").where(name == "George").execute(connection_);

  res = q.select().from("person").where(name == "George").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end());

  q.drop("person").execute(connection_);
}
//void QueryTestUnit::test_foreign_query()
//{
//  connection_.open();
//
//  query<datatypes> q("item");
//
//  using t_object_item = ObjectItem<datatypes>;
//
//  // create item table and insert item
//  result<datatypes> res(q.create().execute(connection_));
//
//  auto itime = time_val_;
//  matador::identifier<unsigned long> id(23);
//  datatypes *hans = new datatypes("Hans", 4711);
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
//    object_ptr<datatypes> i = obj->ptr();
//    UNIT_ASSERT_TRUE(i.has_primary_key());
//    UNIT_ASSERT_TRUE(*i.primary_key() == id);
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
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  person hans("Hans", matador::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(hans).into("person").execute(connection_);

  column name("name");
  res = q.select().from("person").where(name == "hans").execute(connection_);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans");
    UNIT_ASSERT_EQUAL(item->height(), 180U);
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980));

    ++first;
  }

  res = q.select().from("person").where(name == "heinz").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end());

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_query_range_loop()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).into("person").execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).into("person").execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).into("person").execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).into("person").execute(connection_);

  column name("name");
  column height("height");
  res = q.select().from("person").where(name != "Hans" && (height > 160 && height < 180)).execute(connection_);

  std::vector<std::string> result_names({"Hilde", "Trude"});
  unsigned size(0);
  for (auto &&item : res) {
     ++size;
     UNIT_EXPECT_TRUE(contains(result_names, item->name()));
//     UNIT_ASSERT_EQUAL(180U, item->height());
//     UNIT_ASSERT_EQUAL(matador::date(12, 3, 1980), item->birthdate());
  }

  UNIT_ASSERT_EQUAL(size, 2U);

  res = q.select().from("person").where(name == "heinz").execute(connection_);

  UNIT_ASSERT_TRUE(res.begin() == res.end());

  q.drop("person").execute(connection_);

}

void QueryTestUnit::test_query_select()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).into("person").execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).into("person").execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).into("person").execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).into("person").execute(connection_);

  res = q.select().from("person").execute(connection_);

  if (connection_.type() != "mssql") {
    UNIT_ASSERT_EQUAL(4UL, res.size());
  }

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());
    ++first;
  }

  column name("name");
  res = q.select().from("person").where(name == "Hans").execute(connection_);

  if (connection_.type() != "mssql") {
    UNIT_ASSERT_EQUAL(1UL, res.size());
  }

  first = res.begin();
  last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans");
    UNIT_ASSERT_EQUAL(item->height(), 180U);
    UNIT_ASSERT_EQUAL(item->birthdate(), matador::date(12, 3, 1980));

    ++first;
  }

  res = q.select().from("person").order_by("height").asc().execute(connection_);

  first = res.begin();

  bool is_first = true;
  while (first != last) {
    std::unique_ptr<person> item(first.release());

    if (is_first) {
      UNIT_ASSERT_EQUAL(item->name(), "Otto");
      UNIT_ASSERT_EQUAL(item->height(), 159U);
      is_first = false;
    }

    ++first;
  }

  column height("height");
  res = q.select()
    .from("person")
    .where(height > 160 && height < 180)
    .order_by("height")
    .desc()
    .execute(connection_);

  if (connection_.type() != "mssql") {
    UNIT_ASSERT_EQUAL(2UL, res.size());
  }

  first = res.begin();

  is_first = true;
  while (first != last) {
    std::unique_ptr<person> item(first.release());

    if (is_first) {
      UNIT_ASSERT_EQUAL(item->name(), "Hilde");
      UNIT_ASSERT_EQUAL(item->height(), 175U);
      is_first = false;
    }

    ++first;
  }

  res = q.select().from("person").where(name == "Holger").execute(connection_);

  if (connection_.type() != "mssql") {
    UNIT_ASSERT_EQUAL(0UL, res.size());
    UNIT_ASSERT_TRUE(res.empty());
  }

  res = q.drop("person").execute(connection_);
}

void QueryTestUnit::test_query_select_count()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).into("person").execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).into("person").execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).into("person").execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).into("person").execute(connection_);

  query<> count;

  const auto name ="name"_col;
  auto rowres = count.select({columns::count_all()})
                     .from("person")
                     .where(name == "Hans" || name == "Hilde")
                     .execute(connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(2, item->at<int>(0));
    ++first;
  }

  q.drop("person").execute(connection_);

}

void QueryTestUnit::test_query_select_columns()
{
  connection_.connect();

  query<person> q{};

  // create item table and insert item
  result<person> res(q.create("person").execute(connection_));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert(hans).into("person").execute(connection_);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert(otto).into("person").execute(connection_);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert(hilde).into("person").execute(connection_);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert(trude).into("person").execute(connection_);

  column name("name");

  query<> cols;

  auto rowres = cols.select({"id", "name"})
                    .from("person")
                    .where(name == "Hans")
                    .execute(connection_);

  auto first = rowres.begin();
  auto last = rowres.end();

  while (first != last) {
    std::unique_ptr<row> item(first.release());
    UNIT_EXPECT_EQUAL(1UL, item->at<unsigned long>("id"));
    UNIT_EXPECT_EQUAL("Hans", item->at<std::string>(name.name()));
    ++first;
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_query_select_like()
{
  connection_.connect();

  query<child> q{};

  // create item table and insert item
  result<child> res(q.create("child").execute(connection_));

  std::vector<std::string> names({ "height", "light", "night", "dark" });

  unsigned long id(0);
  for (const auto &name : names) {
    child c(name);
    c.id = ++id;
    q.insert(c).into("child").execute(connection_);
  }

  res = q.select()
         .from("child")
         .where(like("name"_col, "%ight%"))
         .execute(connection_);

  std::vector<std::string> result_names({ "height", "light", "night" });

  for (auto ch : res) {
    auto it = std::find(result_names.begin(), result_names.end(), ch->name);
    if (it != result_names.end()) {
      result_names.erase(it);
    }
  }

  UNIT_EXPECT_TRUE(result_names.empty());

  q.drop("child").execute(connection_);
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

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "owner_id", owner);
    matador::access::attribute(op, "item_id", item);
  }
};

void QueryTestUnit::test_select_limit()
{
  connection_.connect();

  query<relation> q{};

  // create item table and insert item
  result<relation> res(q.create("relation").execute(connection_));

  auto r1 = std::make_unique<relation>(1UL, 1UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 1UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 2UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 3UL);
  res = q.insert(*r1).into("relation").execute(connection_);

  res = q.select().from("relation").limit(1).execute(connection_);

  std::size_t count = 0;
  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    ++count;
    ++first;
  }
  UNIT_ASSERT_EQUAL(1UL, count);

  q.drop("relation").execute(connection_);
}

void QueryTestUnit::test_update_limit()
{
  connection_.connect();

  query<relation> q{};

  // create item table and insert item
  result<relation> res(q.create("relation").execute(connection_));

  auto r1 = std::make_unique<relation>(1UL, 1UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 1UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 2UL);
  res = q.insert(*r1).into("relation").execute(connection_);
  r1 = std::make_unique<relation>(1UL, 3UL);
  res = q.insert(*r1).into("relation").execute(connection_);

  matador::column owner("owner_id");
  matador::column item("item_id");
  relation::t_id newid(4UL);
  q.update("relation", {{item.name(), newid}})
    .where("owner_id"_col == 1 && item == 1)
    .limit(1);

  res = q.execute(connection_);

  q.drop("relation").execute(connection_);
}

void QueryTestUnit::test_prepared_statement()
{
  connection_.connect();

  query<> q{};

  q.create("person", {
     make_pk_column<long>("id"),
     make_column("name", 63),
     make_column<unsigned>("age")
  });

  auto stmt = q.prepare(connection_);

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"));
  auto fields = connection_.describe("person");

  auto cols = { "id", "name", "age" };

  for (const auto &fld : fields) {
	  UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end());
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_prepared_statement_creation()
{
  connection_.connect();

  query<person> q{};

  q.create("person");

  auto stmt = q.prepare(connection_);

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"));

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_prepared_object_result_twice()
{
  connection_.connect();

  query<person> q{};

  q.create("person");

  auto stmt = q.prepare(connection_);

  stmt.execute();

  UNIT_ASSERT_TRUE(connection_.exists("person"));

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });

  unsigned long id(0);
  for (const auto& name : names) {
    person p(name, matador::date(12, 3, 1980), 180);
    p.id(++id);
    q.insert(p).into("person").execute(connection_);
  }

  stmt = q.select().from("person").prepare(connection_);

  {
    std::set<std::string> nameset;

    for(const auto &name : names) {
      nameset.insert(name);
    }
    auto result = stmt.execute();

    for (auto p : result) {
      auto i = nameset.find(p->name());
      UNIT_EXPECT_TRUE(i != nameset.end());
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
      UNIT_EXPECT_TRUE(i != nameset.end());
      nameset.erase(i);
    }
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_prepared_scalar_result_twice()
{
  connection_.connect();

  query<> q{};

  q.create("person", {
             make_pk_column<int>("id"),
           });

  q.execute(connection_);

  std::vector<int> ids({ 1,2,3,4 });

  for (int id : ids) {
    q.insert({"id"}).into("person").values({id}).execute(connection_);
  }

  auto stmt = q.select({"id"}).from("person").prepare(connection_);

  std::set<int> idset;

  for(auto id : ids) {
    idset.insert(id);
  }
  auto result = stmt.execute();

  for (auto p : result) {
    auto i = idset.find(p->at<int>("id"));
    UNIT_EXPECT_TRUE(i != idset.end());
    idset.erase(i);
  }

  for(auto id : ids) {
    idset.insert(id);
  }
  /*auto */result = stmt.execute();

  for (auto p : result) {
    auto i = idset.find(p->at<int>("id"));
    UNIT_EXPECT_TRUE(i != idset.end());
    idset.erase(i);
  }

  q.drop("person").execute(connection_);
}

void QueryTestUnit::test_rows()
{
  connection_.connect();

  query<> q{};

  auto cols = std::vector<std::string>{"id", "val_string", "val_varchar",
               "val_char", "val_short", "val_int", "val_long", "val_long_long",
               "val_uchar", "val_ushort", "val_uint", "val_ulong", "val_ulong_long",
               "val_bool",
               "val_float", "val_double", "val_date", "val_time"};

  q.create("item", {
             make_pk_column<int>("id"),
             make_column<std::string>("val_string"),
             make_column<std::string>("val_varchar", 63),
             make_column<char>("val_char"),
             make_column<short>("val_short"),
             make_column<int>("val_int"),
             make_column<long>("val_long"),
             make_column<long long>("val_long_long"),
             make_column<unsigned char>("val_uchar"),
             make_column<unsigned short>("val_ushort"),
             make_column<unsigned int>("val_uint"),
             make_column<unsigned long>("val_ulong"),
             make_column<unsigned long long>("val_ulong_long"),
             make_column<bool>("val_bool"),
             make_column<float>("val_float"),
             make_column<double>("val_double"),
             make_column<matador::date>("val_date"),
             make_column<matador::time>("val_time"),
           });

  q.execute(connection_);

  UNIT_ASSERT_TRUE(connection_.exists("item"));
  auto fields = connection_.describe("item");

  for (const auto &fld : fields) {
    UNIT_EXPECT_FALSE(std::find(cols.begin(), cols.end(), fld.name()) == cols.end());
  }

  int id{1};
  std::string str{"long text"};
  std::string varchar{"good day"};
  char c{-11};
  short s{-256};
  int i{-123456};
  long l{-9876543};
  long long ll{-987654321};
  unsigned char uc{13};
  unsigned short us{1024};
  unsigned int ui{654321};
  unsigned long ul{12345678};
  unsigned long long ull{1234567890};
  bool b{true};
  float f{3.1415f};
  double d{2.71828};
  matador::date md{matador::date()};
  matador::time mt{matador::time::now()};

  q.insert(cols)
   .into("item")
   .values({id, str, varchar, c, s, i, l, ll, uc, us, ui, ul, ull, b, f, d, md, mt})
   .execute(connection_);

  auto res = q.select(cols).from("item").execute(connection_);

  for (auto item : res) {
    UNIT_EXPECT_EQUAL(id, item->at<int>("id"));
    UNIT_EXPECT_EQUAL(str, item->at<std::string>("val_string"));
    // Todo: make compare work
    UNIT_EXPECT_EQUAL(c, item->at<char>("val_char"));
    UNIT_EXPECT_EQUAL(s, item->at<short>("val_short"));
    UNIT_EXPECT_EQUAL(i, item->at<int>("val_int"));
    UNIT_EXPECT_EQUAL(l, item->at<long>("val_long"));
    UNIT_EXPECT_EQUAL(ll, item->at<long long>("val_long_long"));
    // Todo: make compare work
    UNIT_EXPECT_EQUAL(uc, item->at<unsigned char>("val_uchar"));
    UNIT_EXPECT_EQUAL(us, item->at<unsigned short>("val_ushort"));
    UNIT_EXPECT_EQUAL(ui, item->at<unsigned int>("val_uint"));
    UNIT_EXPECT_EQUAL(ul, item->at<unsigned long>("val_ulong"));
    UNIT_EXPECT_EQUAL(ull, item->at<unsigned long long>("val_ulong_long"));
    UNIT_EXPECT_TRUE(item->at<bool>("val_bool"));
    UNIT_EXPECT_EQUAL(f, item->at<float>("val_float"));
    UNIT_EXPECT_EQUAL(d, item->at<double>("val_double"));
  }

  q.drop("item").execute(connection_);

}

void QueryTestUnit::test_log()
{
  UNIT_ASSERT_FALSE(connection_.is_log_enabled());

  connection_.enable_log();

  UNIT_ASSERT_TRUE(connection_.is_log_enabled());

  connection_.connect();

  query<person> q{};

  auto stmt = q.create("person").prepare(connection_);

  UNIT_ASSERT_TRUE(stmt.is_log_enabled());

  stmt.disable_log();

  UNIT_ASSERT_FALSE(stmt.is_log_enabled());
}

connection QueryTestUnit::create_connection()
{
  return connection(db_);
}

std::string QueryTestUnit::db() const {
  return db_;
}
