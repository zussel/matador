#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include "matador/sql/column_definition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/schema.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/string.hpp"

#include "models/types.hpp"

#include "QueryFixture.hpp"

using namespace matador::test;
using namespace matador::sql;

TEST_CASE_METHOD(QueryFixture, "Insert and select basic datatypes", "[query][datatypes]") {
  schema.attach<types>("types");
  auto res = query::create()
    .table<types>("types", schema)
    .execute(db);
  tables_to_drop.emplace("types");
 REQUIRE(res == 0);

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
  auto time_val = matador::time(2015, 3, 15, 13, 56, 23, 123);
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

  res = query::insert()
    .into("types", matador::sql::column_generator::generate<types>(schema, true))
    .values(t)
    .execute(db);
  REQUIRE(res == 1);

  auto result = query::select<types>(schema)
    .from("types")
    .fetch_one<types>(db);
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

TEST_CASE_METHOD(QueryFixture, "Test quoted identifier", "[query][quotes][identifier]") {
  using namespace matador::sql;

  auto res = query::create()
    .table("quotes", {
      make_column<std::string>("from", 255),
      make_column<std::string>("to", 255)
    })
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("quotes");

  // check table description
  std::vector<std::string> column_names = { "from", "to"};
  std::vector<matador::data_type> types = {matador::data_type::type_varchar, matador::data_type::type_varchar};
  const auto columns = db.describe("quotes");

  for (const auto &col : columns) {
    REQUIRE(col.name() == column_names[col.index()]);
    REQUIRE(col.type() == types[col.index()]);
  }

  res = query::insert()
    .into("quotes", {"from", "to"})
    .values({"Berlin", "London"})
    .execute(db);
  REQUIRE(res == 1);

  auto row = query::select({"from", "to"})
    .from("quotes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("from").as<std::string>() == "Berlin");
  REQUIRE(row->at("to").as<std::string>() == "London");

  res = query::update("quotes")
    .set({{"from", "Hamburg"}, {"to", "New York"}})
    .where("from"_col == "Berlin")
    .execute(db);
  REQUIRE(res == 1);

  row = query::select({"from", "to"})
    .from("quotes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("from").as<std::string>() == "Hamburg");
  REQUIRE(row->at("to").as<std::string>() == "New York");
}

TEST_CASE_METHOD(QueryFixture, "Test quoted column names", "[query][quotes][column]") {
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
  tables_to_drop.emplace("quotes");

  for (const auto &name : column_names) {
    auto res = query::create()
      .table("quotes", {
        make_column<std::string>(name, 255),
      })
      .execute(db);
    REQUIRE(res == 0);

    const auto columns = db.describe("quotes");
    for (const auto &col : columns) {
      REQUIRE(col.name() == name);
      REQUIRE(col.type() == matador::data_type::type_varchar);
    }

    res = query::drop()
      .table("quotes")
      .execute(db);
    REQUIRE(res == 0);
  }
}

TEST_CASE_METHOD(QueryFixture, "Test quoted literals", "[query][quotes][literals]") {
  using namespace matador::sql;

  auto res = query::create()
    .table("escapes", {
      make_column<std::string>("name", 255),
    })
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("escapes");

  res = query::insert()
    .into("escapes", {"name"})
    .values({"text"})
    .execute(db);
  REQUIRE(res == 1);

  auto row = query::select({"name"})
    .from("escapes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text");

  res = query::update("escapes")
    .set({{"name", "text'd"}})
    .execute(db);
  REQUIRE(res == 1);

  row = query::select({"name"})
    .from("escapes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text'd");

  res = query::update("escapes")
    .set({{"name", "text\nhello\tworld"}})
    .execute(db);
  REQUIRE(res == 1);

  row = query::select({"name"})
    .from("escapes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text\nhello\tworld");

  res = query::update("escapes")
    .set({{"name", "text \"text\""}})
    .execute(db);
  REQUIRE(res == 1);

  row = query::select({"name"})
    .from("escapes")
    .fetch_one(db);

  REQUIRE(row.has_value());
  REQUIRE(row->at("name").as<std::string>() == "text \"text\"");
}

TEST_CASE_METHOD(QueryFixture, "Test describe table", "[query][describe][table]") {
  using namespace matador::sql;

  schema.attach<types>("types");
  const auto res = query::create()
    .table<types>("types", schema)
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("types");

  const auto columns = db.describe("types");

  std::vector<std::string> column_names = { "id",
                                            "val_char", "val_float", "val_double", "val_short",
                                            "val_int", "val_long", "val_long_long", "val_unsigned_char",
                                            "val_unsigned_short", "val_unsigned_int", "val_unsigned_long", "val_unsigned_long_long",
                                            "val_bool", "val_cstr", "val_string", "val_varchar", "val_date", "val_time",
                                            "val_binary"};
  const std::vector<std::function<bool (const column_definition&)>> type_check = {
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
    const bool b = type_check[col.index()](col);
    REQUIRE(type_check[col.index()](col));
  }
}

TEST_CASE_METHOD(QueryFixture, "Test unknown table", "[query][table]") {
  using Catch::Matchers::ContainsSubstring;
  REQUIRE_THROWS(query::select({"name"})
    .from("person")
    .fetch_all(db));
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

TEST_CASE_METHOD(QueryFixture, "Test primary key", "[query][primary key]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;

  schema.attach<pk>("pk");
  auto res = query::create()
    .table<pk>("pk", schema)
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("pk");

  pk pk1{ 7, "george" };

  res = query::insert()
    .into("pk", column_generator::generate<pk>(schema))
    .values(pk1)
    .execute(db);
  REQUIRE(res == 1);

  auto row = query::select<pk>(schema)
    .from("pk")
    .fetch_one<pk>(db);
  REQUIRE(row != nullptr);
  REQUIRE(row->id > 0);
}

TEST_CASE_METHOD(QueryFixture, "Test primary key prepared", "[query][primary key][prepared]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;

  schema.attach<pk>("pk");
  auto res = query::create()
    .table<pk>("pk", schema)
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("pk");

  pk pk1{ 7, "george" };

  auto stmt = query::insert()
    .into("pk", column_generator::generate<pk>(schema))
    .values<pk>()
    .prepare(db);
  stmt.bind(pk1);

  res = stmt.execute();
  REQUIRE(res == 1);

  stmt = query::select<pk>(schema)
    .from("pk")
    .prepare(db);

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
TEST_CASE_METHOD(QueryFixture, "Test select time and date", "[query][select][time]") {
  using namespace matador::test::temporary;
  using namespace matador::sql;
  schema.attach<appointment>("appointment");
  auto res = query::create()
    .table<appointment>("appointment", schema)
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("appointment");

  auto dinner = appointment{ 1, "dinner" };
  auto time_str = matador::utils::to_string(dinner.time_point);
  auto date_str = matador::utils::to_string(dinner.date_point);

  res = query::insert()
    .into("appointment", column_generator::generate<appointment>(schema))
    .values(dinner)
    .execute(db);
  REQUIRE(res == 1);

  auto row = query::select<appointment>(schema)
    .from("appointment")
    .fetch_one<appointment>(db);

  REQUIRE(row != nullptr);
  REQUIRE(matador::utils::to_string(row->time_point) == time_str);
  REQUIRE(matador::utils::to_string(row->date_point) == date_str);
}

TEST_CASE_METHOD(QueryFixture, "Test null column", "[query][select][null]") {
  using namespace matador::sql;

  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("first_name", 255, null_option::NULLABLE),
      make_column<std::string>("last_name", 255, null_option::NULLABLE)
    })
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  res = query::insert()
    .into("person", {"id", "first_name"})
    .values({1, "george"})
    .execute(db);
  REQUIRE(res == 1);

  res = query::insert()
    .into("person", {"id", "last_name"})
    .values({2, "clooney"})
    .execute(db);
  REQUIRE(res == 1);

  auto result = query::select({"id", "first_name", "last_name"})
    .from("person")
    .fetch_all(db);

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

TEST_CASE_METHOD(QueryFixture, "Test null column prepared", "[query][select][null][prepared]") {
  using namespace matador::sql;

  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("first_name", 255, null_option::NULLABLE),
      make_column<std::string>("last_name", 255, null_option::NULLABLE)
    })
    .execute(db);
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  res = query::insert()
    .into("person", {"id", "first_name"})
    .values({1, "george"})
    .execute(db);
  REQUIRE(res == 1);

  res = query::insert()
    .into("person", {"id", "last_name"})
    .values({2, "clooney"})
    .execute(db);
  REQUIRE(res == 1);

  auto result = query::select({"id", "first_name", "last_name"})
    .from("person")
    .fetch_all(db);

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