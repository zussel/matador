#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include "matador/sql/column_definition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/schema.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/string.hpp"

#include "models/types.hpp"

#include "connection.hpp"

#include <stdexcept>

using namespace matador::test;

class BasicQueryFixture
{
public:
  BasicQueryFixture()
  : db(matador::test::connection::dns)
  , schema(db.dialect().default_schema_name())
  {
    db.open();
  }

  ~BasicQueryFixture()
  {
    drop_table_if_exists("types");
    drop_table_if_exists("quotes");
    drop_table_if_exists("escapes");
    drop_table_if_exists("pk");
    drop_table_if_exists("appointment");
    drop_table_if_exists("person");
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

private:
  void drop_table_if_exists(const std::string &table_name)
  {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }
};

TEST_CASE_METHOD(BasicQueryFixture, "Insert and select basic datatypes", "[query][datatypes]") {
  schema.attach<types>("types");
  db.query(schema).create()
    .table<types>("types")
    .execute();

  float float_value = 2.445566f;
  double double_value = 11111.23433345;
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
  if (db.type() == "sqlite" || db.type() == "postgres") {
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
  matador::time time_val = matador::time(2015, 3, 15, 13, 56, 23, 123);
  matador::utils::blob blob_val {1,2,3,4,5,6,7,8};
//  matador::utils::blob blob_val {1,2,3,4,0,0,5,6,7,8};

  types t {
  1,
  cval, sval, ival, lval, llval,
  ucval, usval, uival, ulval, ullval,
  float_value, double_value,
  bval,
  "Armer schwarzer Kater",
  strval, varcharval,
  date_val, time_val,
  blob_val
  };

  auto res = db.query(schema)
    .insert()
    .into("types", matador::sql::column_generator::generate<types>(schema, true))
    .values(t)
    .execute();
  REQUIRE(res == 1);

  auto result = db.query(schema)
    .select<types>()
    .from("types")
    .fetch_one<types>();
  REQUIRE(result != nullptr);

  REQUIRE(result->id_ == 1);
  REQUIRE(result->char_ == cval);
  REQUIRE(result->short_ == sval);
  REQUIRE(result->int_ == ival);
  REQUIRE(result->long_ == lval);
  REQUIRE(result->long64_ == llval);
  REQUIRE(result->unsigned_char_ == ucval);
  REQUIRE(result->unsigned_short_ == usval);
  REQUIRE(result->unsigned_int_ == uival);
  REQUIRE(result->unsigned_long_ == ulval);
  REQUIRE(result->unsigned_long64_ == ullval);
  REQUIRE(result->float_ == float_value);
  REQUIRE(result->double_ == double_value);
  REQUIRE(strcmp(result->cstr_, cstr) == 0);
  REQUIRE(result->bool_ == bval);
  REQUIRE(result->varchar_ == varcharval);
  REQUIRE(result->string_ == strval);
  REQUIRE(result->date_ == date_val);
  REQUIRE(result->time_ == time_val);
  REQUIRE(result->binary_ == blob_val);
}

TEST_CASE_METHOD(BasicQueryFixture, "Test quoted identifier", "[query][quotes][identifier]") {
  using namespace matador::sql;

  auto res = db.query(schema)
    .create()
    .table("quotes", {
      make_column<std::string>("from", 255),
      make_column<std::string>("to", 255)
    })
    .execute();
  REQUIRE(res == 0);

  // check table description
  std::vector<std::string> column_names = { "from", "to"};
  std::vector<matador::data_type> types = {matador::data_type::type_varchar, matador::data_type::type_varchar};
  const auto columns = db.describe("quotes");

  for (const auto &col : columns) {
    REQUIRE(col.name() == column_names[col.index()]);
    REQUIRE(col.type() == types[col.index()]);
  }

  res = db.query(schema)
    .insert()
    .into("quotes", {"from", "to"})
    .values({"Berlin", "London"})
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select({"from", "to"})
    .from("quotes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("from").as<std::string>() == "Berlin");
  REQUIRE(row->at("to").as<std::string>() == "London");

  res = db.query(schema)
    .update("quotes")
    .set({{"from", "Hamburg"}, {"to", "New York"}})
    .where("from"_col == "Berlin")
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select({"from", "to"})
    .from("quotes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("from").as<std::string>() == "Hamburg");
  REQUIRE(row->at("to").as<std::string>() == "New York");
}

TEST_CASE_METHOD(BasicQueryFixture, "Test quoted column names", "[query][quotes][column]") {
  using namespace matador::sql;

  const auto start_quote = db.dialect().token_at(matador::sql::dialect_token::START_QUOTE);
  const auto end_quote = db.dialect().token_at(matador::sql::dialect_token::END_QUOTE);

  const std::string column_name = "name_with_" + start_quote + "open_close_quotes" + end_quote + "_in_backend_ctx";

  std::vector<std::string> column_names = {
    "normal_name",
    column_name,
    "name_with_'string'_\"literal\"_quotes",
    "name_with_`identifier_quotes`_in_backend_ctx",
    "from"
  };

  for (const auto &name : column_names) {
    auto res = db.query(schema)
      .create()
      .table("quotes", {
        make_column<std::string>(name, 255),
      })
      .execute();
    REQUIRE(res == 0);

    const auto columns = db.describe("quotes");
    for (const auto &col : columns) {
      REQUIRE(col.name() == name);
      REQUIRE(col.type() == matador::data_type::type_varchar);
    }

    res = db.query(schema)
      .drop()
      .table("quotes")
      .execute();
    REQUIRE(res == 0);
  }
}

TEST_CASE_METHOD(BasicQueryFixture, "Test quoted literals", "[query][quotes][literals]") {
  using namespace matador::sql;

  auto res = db.query(schema)
    .create()
    .table("escapes", {
      make_column<std::string>("name", 255),
    })
    .execute();
  REQUIRE(res == 0);

  res = db.query(schema)
    .insert()
    .into("escapes", {"name"})
    .values({"text"})
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select({"name"})
    .from("escapes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text");

  res = db.query(schema)
    .update("escapes")
    .set({{"name", "text'd"}})
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select({"name"})
    .from("escapes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text'd");

  res = db.query(schema)
    .update("escapes")
    .set({{"name", "text\nhello\tworld"}})
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select({"name"})
    .from("escapes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text\nhello\tworld");

  res = db.query(schema)
    .update("escapes")
    .set({{"name", "text \"text\""}})
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select({"name"})
    .from("escapes")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text \"text\"");
}

TEST_CASE_METHOD(BasicQueryFixture, "Test describe table", "[query][describe][table]") {
  using namespace matador::sql;

  schema.attach<types>("types");
  auto res = db.query(schema)
    .create()
    .table<types>("types")
    .execute();
  REQUIRE(res == 0);

  const auto columns = db.describe("types");

  std::vector<std::string> column_names = { "id",
                                            "val_char", "val_float", "val_double", "val_short",
                                            "val_int", "val_long", "val_long_long", "val_unsigned_char",
                                            "val_unsigned_short", "val_unsigned_int", "val_unsigned_long", "val_unsigned_long_long",
                                            "val_bool", "val_cstr", "val_string", "val_varchar", "val_date", "val_time",
                                            "val_binary"};
  std::vector<std::function<bool (const column_definition&)>> type_check = {
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_floating_point(); },
    [](const column_definition &cf) { return cf.is_floating_point(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_integer(); },
    [](const column_definition &cf) { return cf.is_bool(); },
    [](const column_definition &cf) { return cf.is_varchar(); },
    [](const column_definition &cf) { return cf.is_string(); },
    [](const column_definition &cf) { return cf.is_varchar(); },
    [](const column_definition &cf) { return cf.is_date(); },
    [](const column_definition &cf) { return cf.is_time(); },
    [](const column_definition &cf) { return cf.is_blob(); }
  };

  for (const auto &col : columns) {
    REQUIRE(col.name() == column_names[col.index()]);
    REQUIRE(type_check[col.index()](col));
  }
}

TEST_CASE_METHOD(BasicQueryFixture, "Test unknown table", "[query][table]") {
  using Catch::Matchers::ContainsSubstring;
  REQUIRE_THROWS_WITH(db.query(schema).select({"name"}).from("person").fetch_all(), ContainsSubstring("no such table"));
}

namespace matador::test::temporary {
struct pk {
  template<class Operator>
  void process(Operator &op) {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
  }

  unsigned long id{};
  std::string name;
};
}

TEST_CASE_METHOD(BasicQueryFixture, "Test primary key", "[query][primary key]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;

  schema.attach<pk>("pk");
  auto res = db.query(schema)
    .create()
    .table<pk>("pk")
    .execute();
  REQUIRE(res == 0);

  pk pk1{ 7, "george" };

  res = db.query(schema)
    .insert()
    .into("pk", column_generator::generate<pk>(schema))
    .values(pk1)
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select<pk>()
    .from("pk")
    .fetch_one<pk>();
  REQUIRE(row != nullptr);
  REQUIRE(row->id > 0);
}

TEST_CASE_METHOD(BasicQueryFixture, "Test primary key prepared", "[query][primary key][prepared]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;

  schema.attach<pk>("pk");
  auto res = db.query(schema)
    .create()
    .table<pk>("pk")
    .execute();
  REQUIRE(res == 0);

  pk pk1{ 7, "george" };

  auto stmt = db.query(schema)
    .insert()
    .into("pk", column_generator::generate<pk>(schema))
    .values<pk>()
    .prepare();
  stmt.bind(pk1);

  res = stmt.execute();
  REQUIRE(res == 1);

  stmt = db.query(schema)
    .select<pk>()
    .from("pk")
    .prepare();

  auto row = stmt.fetch_one<pk>();
  REQUIRE(row != nullptr);
  REQUIRE(row->id > 0);
}

namespace matador::test::temporary {
struct appointment
{
  unsigned long id{};
  std::string name;
  matador::time time_point{};
  matador::date date_point{};

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "time_point", time_point);
    matador::access::attribute(op, "date_point", date_point);
  }
};

}
TEST_CASE_METHOD(BasicQueryFixture, "Test select time and date", "[query][select][time]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;
  schema.attach<appointment>("appointment");
  auto res = db.query(schema)
    .create()
    .table<appointment>("appointment")
    .execute();
  REQUIRE(res == 0);

  auto dinner = appointment{ 1, "dinner" };
  auto time_str = matador::utils::to_string(dinner.time_point);
  auto date_str = matador::utils::to_string(dinner.date_point);

  res = db.query(schema)
    .insert()
    .into("appointment", column_generator::generate<appointment>(schema))
    .values(dinner)
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select<appointment>()
    .from("appointment")
    .fetch_one<appointment>();

  REQUIRE(row != nullptr);
  REQUIRE(matador::utils::to_string(row->time_point) == time_str);
  REQUIRE(matador::utils::to_string(row->date_point) == date_str);
}

TEST_CASE_METHOD(BasicQueryFixture, "Test null column", "[query][select][null]") {
  using namespace matador::sql;

  auto res = db.query(schema)
    .create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("first_name", 255, null_option::NULLABLE),
      make_column<std::string>("last_name", 255, null_option::NULLABLE)
    })
    .execute();
  REQUIRE(res == 0);

  res = db.query(schema)
    .insert()
    .into("person", {"id", "first_name"})
    .values({1, "george"})
    .execute();
  REQUIRE(res == 1);

  res = db.query(schema)
    .insert()
    .into("person", {"id", "last_name"})
    .values({2, "clooney"})
    .execute();
  REQUIRE(res == 1);

  auto result = db.query(schema)
    .select({"id", "first_name", "last_name"})
    .from("person")
    .fetch_all();

  std::vector<std::string> expected_first_names{"george", ""};
  std::vector<std::string> expected_last_names{"", "clooney"};
  size_t index{0};
  for (const auto& row : result) {
    auto first_name = row.at("first_name").as<std::string>();
    auto last_name = row.at("last_name").as<std::string>();
    REQUIRE(first_name == expected_first_names[index]);
    REQUIRE(last_name == expected_last_names[index++]);
  }
}

TEST_CASE_METHOD(BasicQueryFixture, "Test null column prepared", "[query][select][null][prepared]") {
  using namespace matador::sql;

  auto res = db.query(schema)
    .create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("first_name", 255, null_option::NULLABLE),
      make_column<std::string>("last_name", 255, null_option::NULLABLE)
    })
    .execute();
  REQUIRE(res == 0);

  res = db.query(schema)
    .insert()
    .into("person", {"id", "first_name"})
    .values({1, "george"})
    .execute();
  REQUIRE(res == 1);

  res = db.query(schema)
    .insert()
    .into("person", {"id", "last_name"})
    .values({2, "clooney"})
    .execute();
  REQUIRE(res == 1);

  auto result = db.query(schema)
    .select({"id", "first_name", "last_name"})
    .from("person")
    .fetch_all();

  std::vector<std::string> expected_first_names{"george", ""};
  std::vector<std::string> expected_last_names{"", "clooney"};
  size_t index{0};
  for (const auto& row : result) {
    auto first_name = row.at("first_name").as<std::string>();
    auto last_name = row.at("last_name").as<std::string>();
    REQUIRE(first_name == expected_first_names[index]);
    REQUIRE(last_name == expected_last_names[index++]);
  }
}