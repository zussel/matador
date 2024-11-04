#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query.hpp"

#include "matador/utils/types.hpp"
#include "matador/utils/string.hpp"

#include "QueryFixture.hpp"

#include <list>
#include <algorithm>

using namespace matador::sql;
using namespace matador::test;

TEST_CASE_METHOD(QueryFixture, "Test all data types for record", "[query][record][data types]") {
  check_table_not_exists("types");

  auto res = query::create()
    .table("types", {
      make_pk_column<unsigned long>("id"),
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
      make_column<std::string>("val_string"),
      make_column<std::string>("val_varchar", 63),
      make_column<matador::date>("val_date"),
      make_column<matador::time>("val_time"),
      make_column<matador::utils::blob>("val_blob"),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_exists("types");
  tables_to_drop.emplace("types");

  auto cols = std::vector<std::string>{"id",
                                       "val_char", "val_short", "val_int", "val_long", "val_long_long",
                                       "val_uchar", "val_ushort", "val_uint", "val_ulong", "val_ulong_long",
                                       "val_bool",
                                       "val_float", "val_double",
                                       "val_string", "val_varchar",
                                       "val_date", "val_time", "val_blob"};

  const auto fields = db.describe("types");
  REQUIRE(fields.is_ok());
  for (const auto &fld : *fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }

  unsigned long id{1};
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
  std::string str{"long text"};
  std::string varchar{"good day"};
  auto md{matador::date()};
  auto mt{matador::time::now()};
  matador::utils::blob bin{0x01,0x02,0x03,0x04};

  res = query::insert()
    .into("types", cols)
    .values({id, c, s, i, l, ll, uc, us, ui, ul, ull, b, f, d, str, varchar, md, mt, bin})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 1);

  auto row = query::select(cols)
    .from("types")
    .fetch_one(db);

  REQUIRE(row.is_ok());
  REQUIRE(row.value().has_value());

  REQUIRE(id == (*row)->at<unsigned long>("id"));
  REQUIRE(c == (*row)->at<char>("val_char"));
  REQUIRE(s == (*row)->at<short>("val_short"));
  REQUIRE(i == (*row)->at<int>("val_int"));
  REQUIRE(l == (*row)->at<long>("val_long"));
  REQUIRE(ll == (*row)->at<long long>("val_long_long"));
  REQUIRE(uc == (*row)->at<unsigned char>("val_uchar"));
  REQUIRE(us == (*row)->at<unsigned short>("val_ushort"));
  REQUIRE(ui == (*row)->at<unsigned int>("val_uint"));
  REQUIRE(ul == (*row)->at<unsigned long>("val_ulong"));
  REQUIRE(ull == (*row)->at<unsigned long long>("val_ulong_long"));
  REQUIRE((*row)->at<bool>("val_bool"));
  REQUIRE(f == (*row)->at<float>("val_float"));
  REQUIRE(d == (*row)->at<double>("val_double"));
  REQUIRE(str == (*row)->at<std::string>("val_string"));
  REQUIRE(varchar == (*row)->at<std::string>("val_varchar"));
  REQUIRE(md == (*row)->at<matador::date>("val_date"));
  REQUIRE(mt == (*row)->at<matador::time>("val_time"));
  REQUIRE(bin == (*row)->at<matador::utils::blob>("val_blob"));
}

TEST_CASE_METHOD(QueryFixture, "Create and drop table statement", "[query][record]")
{
  check_table_not_exists("person");
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_exists("person");
  tables_to_drop.emplace("person");

  res = query::drop()
    .table("person")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_not_exists("person");
}

TEST_CASE_METHOD(QueryFixture, "Create and drop table statement with foreign key", "[query][record]")
{
  auto res = query::create()
  .table("airplane", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("brand", 255),
    make_column<std::string>("model", 255),
  })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_exists("airplane");
  tables_to_drop.emplace("airplane");

  res = query::create()
  .table("flight", {
    make_pk_column<unsigned long>("id"),
    make_fk_column<unsigned long>("airplane_id", "airplane", "id"),
    make_column<std::string>("pilot_name", 255),
  })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_exists("flight");
  tables_to_drop.emplace("flight");

  res = query::drop()
    .table("flight")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_not_exists("flight");

  res = query::drop()
    .table("airplane")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);

  check_table_not_exists("airplane");
}

TEST_CASE_METHOD(QueryFixture, "Execute insert record statement", "[query][record]")
{
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("person");

  res = query::insert()
  .into("person", {"id", "name", "age"})
  .values({7, "george", 45})
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 1);

  auto result = query::select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);

  REQUIRE(result.is_ok());
  for (const auto &i: *result) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).template as<long long>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).template as<std::string>() == "george");
    REQUIRE(i.at(2).name() == "age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).template as<int>() == 45);
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute insert record statement with foreign key", "[query][record]")
{
  auto res = query::create()
    .table("airplane", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("brand", 255),
      make_column<std::string>("model", 255),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("airplane");

  res = query::create()
    .table("flight", {
      make_pk_column<unsigned long>("id"),
      make_fk_column<unsigned long>("airplane_id", "airplane", "id"),
      make_column<std::string>("pilot_name", 255),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("flight");

  std::vector<std::vector<matador::utils::any_type>> values_list{
    {1, "Airbus", "A380"},
    {2, "Boeing", "707"},
    {3, "Boeing", "747"}
  };

  for(auto &&values : values_list) {
    res = query::insert()
      .into("airplane", {"id", "brand", "model"})
      .values(std::move(values))
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res.value() == 1);
  }

  auto count = query::select({count_all()})
    .from("airplane")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(count->has_value());
  REQUIRE(*count == 3);

  res = query::insert()
    .into("flight", {"id", "airplane_id", "pilot_name"})
    .values({4, 1, "George"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 1);
}

TEST_CASE_METHOD(QueryFixture, "Execute update record statement", "[query][record]")
{
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("person");

  res = query::insert()
    .into("person", {"id", "name", "age"})
    .values({7, "george", 45})
    .execute(db);

  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 1);

  res = query::update("person")
    .set({{"id",   7},
          {"name", "jane"},
          {"age",  35}})
    .where("id"_col == 7)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 1);

  auto result = query::select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);

  REQUIRE(result.is_ok());
  for (const auto &i: *result) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<long long>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == "jane");
    REQUIRE(i.at(2).name() == "age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).as<int>() == 35);
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement", "[query][record]")
{
  auto res = query::create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<matador::utils::any_type>> values_list{
    {1, "george", 45},
    {2, "jane", 32},
    {3, "michael", 67},
    {4, "bob", 13}
  };

  for(auto &&values : values_list) {
    res = query::insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res.value() == 1);
  }

  auto result = query::select({"id", "name", "age"})
    .from("person")
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::list<std::string> expected_names{"george", "jane", "michael", "bob"};
  for (const auto &p: *result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());

  auto rec = query::select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(rec.is_ok());
  REQUIRE(rec->has_value());
  REQUIRE((*rec)->at(1).str() == "george");

  auto name = query::select({"name"})
  .from("person")
  .fetch_value<std::string>(db);

  REQUIRE(name.is_ok());
  REQUIRE(*name == "george");
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with order by", "[query][record]")
{
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res.value() == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<matador::utils::any_type>> values_list{
    {1, "george", 45},
    {2, "jane", 32},
    {3, "michael", 67},
    {4, "bob", 13}
  };

  for(auto &&values : values_list) {
    res = query::insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res.value() == 1);
  }

  auto result = query::select({"id", "name", "age"})
    .from("person")
    .order_by("name").asc()
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::list<std::string> expected_names{"bob", "george", "jane", "michael"};
  for (const auto &p: *result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with group by and order by", "[query][record]")
{
  auto res = query::create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
  tables_to_drop.emplace("person");

  std::vector<std::vector<matador::utils::any_type>> values_list{
    {1, "george", 45},
    {2, "jane", 45},
    {3, "joe", 45},
    {4, "michael", 13},
    {5, "bob", 13},
    {6, "charlie", 67}
  };

  for(auto &&values : values_list) {
    res = query::insert()
    .into("person", {"id", "name", "age"})
    .values(std::move(values))
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res.value() == 1);
  }

  auto result = query::select({count("age").as("age_count"), "age"})
    .from("person")
    .group_by("age")
    .order_by("age_count").desc()
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

TEST_CASE_METHOD(QueryFixture, "Execute delete statement", "[query][record]")
{
  auto res = query::create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  }).execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
  tables_to_drop.emplace("person");

  res = query::insert()
    .into("person", {"id", "name", "age"})
    .values({1, "george", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);
  res = query::insert()
    .into("person", {"id", "name", "age"})
    .values({2, "jane", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  auto count = query::select({count_all()})
    .from("person")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());

  REQUIRE(*count == 2);

  res = query::remove()
    .from("person")
    .where("id"_col == 1)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  count = query::select({count_all()})
    .from("person")
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 1);
}

TEST_CASE_METHOD(QueryFixture, "Test quoted identifier record", "[query][record]") {
  auto res = query::create()
    .table("quotes", {
      make_column<std::string>("from", 255),
      make_column<std::string>("to", 255)
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
  tables_to_drop.emplace("quotes");

  // check table description
  std::vector<std::string> columns = { "from", "to"};
  std::vector<matador::data_type> types = {
    matador::data_type::type_varchar,
    matador::data_type::type_varchar
  };
  auto fields = db.describe("quotes");
  REQUIRE(fields.is_ok());

  for (const auto &field : *fields) {
    REQUIRE(field.name() == columns[field.index()]);
    REQUIRE(field.type() == types[field.index()]);
  }

  res = query::insert()
    .into("quotes", {"from", "to"})
    .values({"Berlin", "London"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  auto result = query::select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(result.is_ok());

  REQUIRE(result->has_value());
  REQUIRE("Berlin" == (*result)->at("from").str());
  REQUIRE("London" == (*result)->at("to").str());

  res = query::update("quotes")
    .set({{"from", "Hamburg"}, {"to", "New York"}})
    .where("from"_col == "Berlin")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  result = query::select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(result.is_ok());

  REQUIRE("Hamburg" == (*result)->at("from").str());
  REQUIRE("New York" == (*result)->at("to").str());
}

TEST_CASE_METHOD(QueryFixture, "Test create record", "[query][record][create]") {
  check_table_not_exists("person");
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
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
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
  tables_to_drop.emplace("person");

  check_table_exists("person");

  res = query::insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  auto row = query::select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<unsigned long>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "hans");
  REQUIRE((*row)->at("age").as<unsigned short>() == 45);
}

TEST_CASE_METHOD(QueryFixture, "Test update record", "[query][record][update]") {
  check_table_not_exists("person");
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
  tables_to_drop.emplace("person");

  check_table_exists("person");

  res = query::insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  auto row = query::select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<unsigned long>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "hans");
  REQUIRE((*row)->at("age").as<unsigned short>() == 45);

  res = query::update("person")
    .set({{"name", "jane"}, {"age", 47}})
    .where("name"_col == "hans")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 1);

  row = query::select({"id", "name", "age"})
    .from("person")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE((*row)->at("id").as<unsigned long>() == 1);
  REQUIRE((*row)->at("name").as<std::string>() == "jane");
  REQUIRE((*row)->at("age").as<unsigned short>() == 47);
}

TEST_CASE_METHOD(QueryFixture, "Test prepared record statement", "[query][record][prepared]") {
  check_table_not_exists("person");
  auto stmt = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .prepare(db);
  tables_to_drop.emplace("person");

  auto res = stmt.execute();
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);
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
  auto res = query::create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(*res == 0);

  check_table_exists("person");
  tables_to_drop.emplace("person");

  std::vector<unsigned long> ids({ 1,2,3,4 });

  for(auto id : ids) {
    res = query::insert()
      .into("person", {"id"})
      .values({id})
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(*res == 1);
  }

  auto stmt = query::select({"id"})
    .from("person")
    .order_by("id"_col).asc()
    .prepare(db);

  auto rows = stmt.fetch();
  REQUIRE(rows.is_ok());

  size_t index{0};
  for (const auto &row : *rows) {
    REQUIRE(row.at("id").as<unsigned long>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt.reset();

  rows = stmt.fetch();
  REQUIRE(rows.is_ok());

  index = 0;
  for (const auto &row : *rows) {
    REQUIRE(row.at("id").as<unsigned long>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt.reset();

  auto row = stmt.fetch_one();
  REQUIRE(row.is_ok());
  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("id").as<unsigned long>() == ids[0]);
}