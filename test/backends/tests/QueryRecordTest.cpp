#include <catch2/catch_test_macros.hpp>

#include "matador/query/runtime/connection.hpp"

#include "matador/query/builder/builder.hpp"
#include "matador/query/criteria.hpp"
#include "matador/query/builder/query.hpp"
#include "matador/query/model/column.hpp"

#include "matador/utils/types.hpp"
#include "matador/utils/string.hpp"

#include "fixtures/QueryFixture.hpp"

#include <list>
#include <algorithm>

using namespace matador::query;
using namespace matador::test;
using namespace matador::utils;

TEST_CASE_METHOD(QueryFixture, "Test all data types for record", "[query][record][data types]") {
  check_table_not_exists("types");

  const table tab("types");

  auto res = create()
    .table(tab)
    .columns({
        col("id", basic_type::UInt32),
        col("val_char", basic_type::Int8),
        col("val_short", basic_type::Int16),
        col("val_int", basic_type::Int32),
        col("val_long_long", basic_type::Int64),
        col("val_uchar", basic_type::UInt8),
        col("val_ushort", basic_type::UInt16),
        col("val_uint", basic_type::UInt32),
        col("val_ulong_long", basic_type::UInt64),
        col("val_bool", basic_type::Boolean),
        col("val_float", basic_type::Float),
        col("val_double", basic_type::Double),
        col("val_string", basic_type::Text),
        col("val_varchar", basic_type::Varchar, 63),
        col("val_date", basic_type::Date),
        col("val_time", basic_type::Time),
        col("val_blob", basic_type::Blob),
    })
    .constraints({
        constraint("PK_types").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("types");
  tables_to_drop.emplace("types");

  auto cols = std::vector<std::string>{"id",
                                       "val_char", "val_short", "val_int", "val_long_long",
                                       "val_uchar", "val_ushort", "val_uint", "val_ulong_long",
                                       "val_bool",
                                       "val_float", "val_double",
                                       "val_string", "val_varchar",
                                       "val_date", "val_time",
                                       "val_blob"};

  const auto fields = db.describe("types");
  REQUIRE(fields.is_ok());
  for (const auto &fld : *fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }

  uint32_t id{1};
  int8_t c{-11};
  int16_t s{-256};
  int32_t i{-123456};
  int64_t ll{-987654321};
  uint8_t uc{13};
  uint16_t us{1024};
  uint32_t ui{654321};
  uint64_t ull{1234567890};
  bool b{true};
  float f{3.1415f};
  double d{2.71828};
  std::string str{"long text"};
  std::string varchar{"good day"};
  date_type_t md{2025, 11, 27};
  time_type_t mt{12, 34, 56};
  blob_type_t bin{0x01,0x02,0x03,0x04};

  res = insert()
    .into("types", cols)
    .values({id, c, s, i, ll, uc, us, ui, ull, b, f, d, str, varchar, md, mt, bin})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto result = select(cols)
    .from("types")
    .fetch_one(db);

  REQUIRE(result.is_ok());
  REQUIRE(result.value().has_value());

  const auto &row = result.value().value();
  REQUIRE(id == row.at<uint32_t>("id"));
  REQUIRE(c == row.at<int8_t>("val_char"));
  REQUIRE(s == row.at<int16_t>("val_short"));
  REQUIRE(i == row.at<int32_t>("val_int"));
  REQUIRE(ll == row.at<int64_t>("val_long_long"));
  REQUIRE(uc == row.at<uint8_t>("val_uchar"));
  REQUIRE(us == row.at<unsigned short>("val_ushort"));
  REQUIRE(ui == row.at<unsigned int>("val_uint"));
  REQUIRE(ull == row.at<uint64_t>("val_ulong_long"));
  REQUIRE(row.at<bool>("val_bool"));
  REQUIRE(f == row.at<float>("val_float"));
  REQUIRE(d == row.at<double>("val_double"));
  REQUIRE(str == row.at<std::string>("val_string"));
  REQUIRE(varchar == row.at<std::string>("val_varchar"));
  REQUIRE(md == row.at<date_type_t>("val_date"));
  REQUIRE(mt == row.at<time_type_t>("val_time"));
  REQUIRE(bin == row.at<blob_type_t>("val_blob"));
}

TEST_CASE_METHOD(QueryFixture, "Create and drop table statement", "[query][record]")
{
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("person");
  tables_to_drop.emplace("person");

  res = drop()
    .table("person")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_not_exists("person");
}

TEST_CASE_METHOD(QueryFixture, "Create table with identity primary key", "[query][record][create][identity]") {
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32).primary_key().identity(),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("person");
  tables_to_drop.emplace("person");

  res = drop()
    .table("person")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_not_exists("person");
}

TEST_CASE_METHOD(QueryFixture, "Create and drop table statement with foreign key", "[query][record]")
{
  auto res = create()
  .table("airplane")
    .columns({
        col("id", basic_type::UInt32),
        col("brand", basic_type::Varchar, 255),
        col("model", basic_type::Varchar, 255),
    })
    .constraints({
        constraint("PK_airplane").primary_key("id"),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("airplane");
  tables_to_drop.emplace("airplane");

  res = create()
  .table("flight")
    .columns({
        col("id", basic_type::UInt32),
        col("airplane_id", basic_type::UInt32),
        col("pilot_name", basic_type::Varchar, 255)
    })
    .constraints({
        constraint("PK_flight").primary_key("id"),
        constraint( "FK_flight_airplane_id").foreign_key("airplane_id").references("airplane", "id")
    })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("flight");
  tables_to_drop.emplace("flight");

  res = drop()
    .table("flight")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_not_exists("flight");

  res = drop()
    .table("airplane")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_not_exists("airplane");
}

TEST_CASE_METHOD(QueryFixture, "Execute insert record statement", "[query][record]") {
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  res = insert()
  .into("person", {"id", "name", "age"})
  .values({7, "george", 45})
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto result = select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);

  REQUIRE(result.is_ok());
  for (const auto &i: *result) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).template as<uint32_t>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).template as<std::string>() == "george");
    REQUIRE(i.at(2).name() == "age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).template as<uint32_t>() == 45);
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute insert record statement with foreign key", "[query][record]")
{
  auto res = create()
    .table("airplane")
    .columns({
        col("id", basic_type::UInt32),
        col("brand", basic_type::Varchar, 255),
        col("model", basic_type::Varchar, 255),
    })
    .constraints({
        constraint("PK_airplane").primary_key("id"),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("airplane");

  res = create()
    .table("flight")
      .columns({
          col("id", basic_type::UInt32),
          col("airplane_id", basic_type::UInt32),
          col("pilot_name", basic_type::Varchar, 255)
      })
      .constraints({
          constraint("PK_flight").primary_key("id"),
          constraint( "FK_flight_airplane_id").foreign_key("airplane_id").references("airplane", "id")
      })
      .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("flight");

  std::vector<std::vector<database_type>> values_list{
    {1, "Airbus", "A380"},
    {2, "Boeing", "707"},
    {3, "Boeing", "747"}
  };

  for(auto &&values : values_list) {
    res = insert()
      .into("airplane", {"id", "brand", "model"})
      .values(std::move(values))
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from("airplane")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(count->has_value());
  REQUIRE(*count == 3);

  res = insert()
    .into("flight", {"id", "airplane_id", "pilot_name"})
    .values({4, 1, "George"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);
}

TEST_CASE_METHOD(QueryFixture, "Execute update record statement", "[query][record]")
{
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  res = insert()
    .into("person", {"id", "name", "age"})
    .values({7, "george", 45})
    .execute(db);

  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  res = update("person")
    .set("id",   7)
    .set("name", "jane")
    .set("age",  35)
    .where("id"_col == 7)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto result = select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);

  REQUIRE(result.is_ok());
  for (const auto &i: *result) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<uint32_t>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == "jane");
    REQUIRE(i.at(2).name() == "age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).as<uint32_t>() == 35);
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement", "[query][record]")
{
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<database_type>> values_list{
    {1, "george", 45},
    {2, "jane", 32},
    {3, "michael", 67},
    {4, "bob", 13}
  };

  for(auto &&values : values_list) {
    res = insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto result = select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::list<std::string> expected_names{"george", "jane", "michael", "bob"};
  for (const auto &p: *result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());

  auto rec = select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(rec.is_ok());
  REQUIRE(rec->has_value());
  REQUIRE((*rec)->at(1).str() == "george");

  auto name = select({"name"})
  .from("person")
  .fetch_value<std::string>(db);

  REQUIRE(name.is_ok());
  REQUIRE(*name == "george");
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with order by", "[query][record]")
{
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<database_type>> values_list{
    {1, "george", 45},
    {2, "jane", 32},
    {3, "michael", 67},
    {4, "bob", 13}
  };

  for(auto &&values : values_list) {
    res = insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto result = select({"id", "name", "age"})
    .from("person")
    .order_by("name"_col).asc()
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::list<std::string> expected_names{"bob", "george", "jane", "michael"};
  for (const auto &p: *result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with group by and order by", "[query][record]") {
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<database_type>> values_list{
    {1, "george", 45},
    {2, "jane", 45},
    {3, "joe", 45},
    {4, "michael", 13},
    {5, "bob", 13},
    {6, "charlie", 67}
  };

  for(auto &&values : values_list) {
    res = insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto result = select({count("age").as("age_count"), "age"_col})
    .from("person")
    .group_by({"age"_col})
    .order_by("age_count"_col).desc()
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::list<std::pair<int, int>> expected_values{{3, 45},
                                                 {2, 13},
                                                 {1, 67}};
  for (const auto &r: *result) {
    const auto age_count_val = r.at<int>(0);
    const auto age_val = r.at<int>(1);
    REQUIRE(age_count_val == expected_values.front().first);
    REQUIRE(age_val == expected_values.front().second);
    expected_values.pop_front();
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute delete statement", "[query][record]") {
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  res = insert()
    .into("person", {"id", "name", "age"})
    .values({1, "george", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);
  res = insert()
    .into("person", {"id", "name", "age"})
    .values({2, "jane", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto count = select({count_all()})
    .from("person")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());

  REQUIRE(*count == 2);

  res = remove()
    .from("person")
    .where("id"_col == 1)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  count = select({count_all()})
    .from("person")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 1);
}

TEST_CASE_METHOD(QueryFixture, "Test quoted identifier record", "[query][record]") {
  auto res = create()
    .table("quotes")
    .columns({
        col("from", basic_type::Varchar, 255),
        col("to", basic_type::Varchar, 255)
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("quotes");

  // check table description
  std::vector<std::string> columns = { "from", "to"};
  std::vector types = {
    basic_type::Varchar,
    basic_type::Varchar
  };
  auto fields = db.describe("quotes");
  REQUIRE(fields.is_ok());

  size_t i = 0;
  for (const auto &field : *fields) {
    REQUIRE(field.name() == columns[i]);
    REQUIRE(field.type() == types[i]);
    ++i;
  }

  res = insert()
    .into("quotes", {"from", "to"})
    .values({"Berlin", "London"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto result = select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(result.is_ok());

  REQUIRE(result->has_value());
  REQUIRE("Berlin" == (*result)->at("from").str());
  REQUIRE("London" == (*result)->at("to").str());

  res = update("quotes")
    .set("from", "Hamburg")
    .set("to", "New York")
    .where("from"_col == "Berlin")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  result = select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(result.is_ok());

  REQUIRE("Hamburg" == (*result)->at("from").str());
  REQUIRE("New York" == (*result)->at("to").str());
}

TEST_CASE_METHOD(QueryFixture, "Test create record", "[query][record][create]") {
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  check_table_exists("person");
  const std::vector<std::string> cols = {"id", "name", "age"};
  const auto fields = db.describe("person");
  REQUIRE(fields.is_ok());

  for (const auto &fld : *fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryFixture, "Test insert record", "[query][record][insert]") {
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  check_table_exists("person");

  res = insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<uint32_t>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "hans");
  REQUIRE((*row)->at("age").as<unsigned short>() == 45);
}

TEST_CASE_METHOD(QueryFixture, "Test update record", "[query][record][update]") {
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("person");

  check_table_exists("person");

  res = insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<uint32_t>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "hans");
  REQUIRE((*row)->at("age").as<unsigned short>() == 45);

  res = update("person")
    .set("name", "jane")
    .set("age", 47)
    .where("name"_col == "hans")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<uint32_t>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "jane");
  REQUIRE((*row)->at("age").as<unsigned short>() == 47);
}

TEST_CASE_METHOD(QueryFixture, "Test prepared record statement", "[query][record][prepared]") {
  check_table_not_exists("person");
  auto stmt = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .prepare(db);
  REQUIRE(stmt.is_ok());
  tables_to_drop.emplace("person");

  auto res = stmt->execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  check_table_exists("person");

  const std::vector<std::string> cols = {"id", "name", "age"};
  const auto fields = db.describe("person");
  REQUIRE(fields.is_ok());

  for (const auto &fld : *fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryFixture, "Test scalar result", "[query][record][scalar][result]") {
  check_table_not_exists("person");
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  check_table_exists("person");
  tables_to_drop.emplace("person");

  std::vector<uint32_t> ids({ 1,2,3,4 });

  for(auto id : ids) {
    res = insert()
      .into("person", {"id"})
      .values({id})
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto stmt = select({"id"})
    .from("person")
    .order_by("id"_col).asc()
    .prepare(db);
  REQUIRE(stmt);

  auto rows = stmt->fetch();
  REQUIRE(rows.is_ok());

  size_t index{0};
  for (const auto &row : *rows) {
    REQUIRE(row.at("id").as<uint32_t>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt->reset();

  rows = stmt->fetch();
  REQUIRE(rows.is_ok());

  index = 0;
  for (const auto &row : *rows) {
    REQUIRE(row.at("id").as<uint32_t>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt->reset();

  auto row = stmt->fetch_one();
  REQUIRE(row.is_ok());
  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("id").as<uint32_t>() == ids[0]);
}