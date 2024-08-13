#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query_builder.hpp"

#include "connection.hpp"

#include <list>
#include <algorithm>

class QueryRecordFixture
{
public:
  QueryRecordFixture()
  : db(matador::test::connection::dns)
  , schema(db.dialect().default_schema_name())
  {
    db.open();
  }
  ~QueryRecordFixture() {
    drop_table_if_exists("flight");
    drop_table_if_exists("airplane");
    drop_table_if_exists("person");
    drop_table_if_exists("quotes");
    drop_table_if_exists("types");
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

private:
  void drop_table_if_exists(const std::string &table_name) {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }
};

using namespace matador::sql;

TEST_CASE_METHOD(QueryRecordFixture, "Test all data types for record", "[query][record][data types]") {
  REQUIRE(!db.exists("types"));
  db.query(schema).create()
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
    .execute();

  REQUIRE(db.exists("types"));

  auto cols = std::vector<std::string>{"id",
                                       "val_char", "val_short", "val_int", "val_long", "val_long_long",
                                       "val_uchar", "val_ushort", "val_uint", "val_ulong", "val_ulong_long",
                                       "val_bool",
                                       "val_float", "val_double",
                                       "val_string", "val_varchar",
                                       "val_date", "val_time", "val_blob"};

  const auto fields = db.describe("types");
  for (const auto &fld : fields) {
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
  matador::date md{matador::date()};
  matador::time mt{matador::time::now()};
  matador::utils::blob  bin{1,2,3,4};

  auto res = db.query(schema)
    .insert()
    .into("types", cols)
    .values({id, c, s, i, l, ll, uc, us, ui, ul, ull, b, f, d, str, varchar, md, mt, bin})
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select(cols)
    .from("types")
    .fetch_one();

  REQUIRE(row.has_value());

  REQUIRE(id == row->at<unsigned long>("id"));
  REQUIRE(c == row->at<char>("val_char"));
  REQUIRE(s == row->at<short>("val_short"));
  REQUIRE(i == row->at<int>("val_int"));
  REQUIRE(l == row->at<long>("val_long"));
  REQUIRE(ll == row->at<long long>("val_long_long"));
  REQUIRE(uc == row->at<unsigned char>("val_uchar"));
  REQUIRE(us == row->at<unsigned short>("val_ushort"));
  REQUIRE(ui == row->at<unsigned int>("val_uint"));
  REQUIRE(ul == row->at<unsigned long>("val_ulong"));
  REQUIRE(ull == row->at<unsigned long long>("val_ulong_long"));
  REQUIRE(row->at<bool>("val_bool"));
  REQUIRE(f == row->at<float>("val_float"));
  REQUIRE(d == row->at<double>("val_double"));
  REQUIRE(str == row->at<std::string>("val_string"));
  REQUIRE(varchar == row->at<std::string>("val_varchar"));
  REQUIRE(md == row->at<matador::date>("val_date"));
  REQUIRE(mt == row->at<matador::time>("val_time"));
  REQUIRE(bin == row->at<matador::utils::blob>("val_blob"));
}

TEST_CASE_METHOD(QueryRecordFixture, "Create and drop table statement", "[query][record]")
{
  REQUIRE(!db.exists("person"));
  db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute();

  REQUIRE(db.exists("person"));

  db.query(schema).drop()
    .table("person")
    .execute();

  REQUIRE(!db.exists("person"));
}

TEST_CASE_METHOD(QueryRecordFixture, "Create and drop table statement with foreign key", "[query][record]")
{
  db.query(schema).create()
  .table("airplane", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("brand", 255),
    make_column<std::string>("model", 255),
  })
  .execute();

  REQUIRE(db.exists("airplane"));

  db.query(schema).create()
  .table("flight", {
    make_pk_column<unsigned long>("id"),
    make_fk_column<unsigned long>("airplane_id", "airplane", "id"),
    make_column<std::string>("pilot_name", 255),
  })
  .execute();

  REQUIRE(db.exists("flight"));

  db.query(schema).drop()
  .table("flight")
  .execute();

  REQUIRE(!db.exists("flight"));

  db.query(schema).drop()
  .table("airplane")
  .execute();

  REQUIRE(!db.exists("airplane"));
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute insert record statement", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute();

  auto res = db.query(schema).insert()
  .into("person", {"id", "name", "age"})
  .values({7, "george", 45})
  .execute();

  REQUIRE(res == 1);

  auto result = db.query(schema).select({"id", "name", "age"})
  .from("person")
  .fetch_all();

  for (const auto &i: result) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    //    REQUIRE(i.at(0).type() == data_type_t::type_long_long);
    REQUIRE(i.at(0).template as<long long>() == 7);
    REQUIRE(i.at(1).name() == "name");
    //    REQUIRE(i.at(1).type() == data_type_t::type_varchar);
    REQUIRE(i.at(1).template as<std::string>() == "george");
    REQUIRE(i.at(2).name() == "age");
    //    REQUIRE(i.at(2).type() == matador::sql::data_type_t::type_int);
    REQUIRE(i.at(2).template as<int>() == 45);
  }

  db.query(schema).drop()
  .table("person")
  .execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute insert record statement with foreign key", "[query][record]")
{
  db.query(schema).create()
  .table("airplane", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("brand", 255),
    make_column<std::string>("model", 255),
  })
  .execute();

  db.query(schema).create()
  .table("flight", {
    make_pk_column<unsigned long>("id"),
    make_fk_column<unsigned long>("airplane_id", "airplane", "id"),
    make_column<std::string>("pilot_name", 255),
  })
  .execute();

  auto res = db.query(schema).insert().into("airplane", {"id", "brand", "model"}).values({1, "Airbus", "A380"}).execute();
  REQUIRE(res == 1);

  res = db.query(schema).insert().into("airplane", {"id", "brand", "model"}).values({2, "Boeing", "707"}).execute();
  REQUIRE(res == 1);

  res = db.query(schema).insert().into("airplane", {"id", "brand", "model"}).values({3, "Boeing", "747"}).execute();
  REQUIRE(res == 1);

  auto count = db.query(schema).select({count_all()}).from("airplane").fetch_value<int>();
  REQUIRE(count == 3);

  res = db.query(schema).insert().into("flight", {"id", "airplane_id", "pilot_name"}).values({4, 1, "George"}).execute();
  REQUIRE(res == 1);

  db.query(schema).drop().table("flight").execute();
  db.query(schema).drop().table("airplane").execute();

  REQUIRE(!db.exists("flight"));
  REQUIRE(!db.exists("airplane"));
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute update record statement", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute();

  auto res = db.query(schema).insert()
  .into("person", {"id", "name", "age"})
  .values({7, "george", 45})
  .execute();

  REQUIRE(res == 1);

  res = db.query(schema).update("person")
  .set({{"id",   7},
        {"name", "jane"},
        {"age",  35}})
  .where("id"_col == 7)
  .execute();

  REQUIRE(res == 1);

  auto result = db.query(schema).select({"id", "name", "age"})
  .from("person")
  .fetch_all();

  for (const auto &i: result) {
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

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute select statement", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute();

  auto res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({1, "george", 45}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({2, "jane", 32}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({3, "michael", 67}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({4, "bob", 13}).execute();
  REQUIRE(res == 1);

  auto result = db.query(schema).select({"id", "name", "age"})
  .from("person")
  .fetch_all();

  std::list<std::string> expected_names{"george", "jane", "michael", "bob"};
  for (const auto &p: result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());

  auto rec = db.query(schema).select({"id", "name", "age"})
  .from("person")
  .fetch_one();
  REQUIRE(rec.has_value());
  REQUIRE(rec->at(1).str() == "george");

  auto name = db.query(schema).select({"name"})
  .from("person")
  .fetch_value<std::string>();
  REQUIRE(name == "george");

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute select statement with order by", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute();

  auto res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({1, "george", 45}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({2, "jane", 32}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({3, "michael", 67}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({4, "bob", 13}).execute();
  REQUIRE(res == 1);

  auto result = db.query(schema).select({"id", "name", "age"})
  .from("person")
  .order_by("name").asc()
  .fetch_all();

  std::list<std::string> expected_names{"bob", "george", "jane", "michael"};
  for (const auto &p: result) {
    REQUIRE(p.at(1).str() == expected_names.front());
    expected_names.pop_front();
  }
  REQUIRE(expected_names.empty());

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute select statement with group by and order by", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  })
  .execute();

  auto res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({1, "george", 45}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({2, "jane", 45}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({3, "michael", 13}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({4, "bob", 13}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({5, "charlie", 67}).execute();
  REQUIRE(res == 1);

  auto qc = db.query(schema).select({count("age").as("age_count"), "age"})
  .from("person")
  .group_by("age")
  .order_by("age_count").desc()
  .build();

  auto result = db.query(schema).select({count("age").as("age_count"), "age"})
  .from("person")
  .group_by("age")
  .order_by("age_count").desc()
  .fetch_all();

  std::list<std::pair<int, int>> expected_values{{2, 45},
                                                 {2, 13},
                                                 {1, 67}};
  for (const auto &r: result) {
    REQUIRE(r.at(0).as<int>() == expected_values.front().first);
    REQUIRE(r.at(1).as<int>() == expected_values.front().second);
    expected_values.pop_front();
  }

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Execute delete statement", "[query][record]")
{
  db.query(schema).create()
  .table("person", {
    make_pk_column<unsigned long>("id"),
    make_column<std::string>("name", 255),
    make_column<unsigned short>("age")
  }).execute();

  auto res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({1, "george", 45}).execute();
  REQUIRE(res == 1);
  res = db.query(schema).insert().into("person", {"id", "name", "age"}).values({2, "jane", 45}).execute();
  REQUIRE(res == 1);

  auto count = db.query(schema).select({count_all()}).from("person").fetch_value<int>();
  REQUIRE(count == 2);

  res = db.query(schema).remove()
  .from("person")
  .where("id"_col == 1)
  .execute();

  REQUIRE(res == 1);

  count = db.query(schema).select({count_all()}).from("person").fetch_value<int>();
  REQUIRE(count == 1);

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Test quoted identifier record", "[query][record]") {
  db.query(schema).create()
  .table("quotes", {
    make_column<std::string>("from", 255),
    make_column<std::string>("to", 255)
  }).execute();

  // check table description
  std::vector<std::string> columns = { "from", "to"};
  std::vector<matador::data_type> types = {matador::data_type::type_varchar, matador::data_type::type_varchar};
  auto fields = db.describe("quotes");

  for (const auto &field : fields) {
    REQUIRE(field.name() == columns[field.index()]);
    REQUIRE(field.type() == types[field.index()]);
  }

  db.query(schema).insert().into("quotes", {"from", "to"}).values({"Berlin", "London"}).execute();

  auto res = db.query(schema).select({"from", "to"}).from("quotes").fetch_one();

  REQUIRE(res.has_value());
  REQUIRE("Berlin" == res->at("from").str());
  REQUIRE("London" == res->at("to").str());

  db.query(schema).update("quotes").set({{"from", "Hamburg"}, {"to", "New York"}}).where("from"_col == "Berlin").execute();

  res = db.query(schema).select({"from", "to"}).from("quotes").fetch_one();

  REQUIRE("Hamburg" == res->at("from").str());
  REQUIRE("New York" == res->at("to").str());

  db.query(schema).drop().table("quotes").execute();
}

TEST_CASE_METHOD(QueryRecordFixture, "Test create record", "[query][record][create]") {
  REQUIRE(!db.exists("person"));
  db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute();

  REQUIRE(db.exists("person"));
  const std::vector<std::string> cols = {"id", "name", "age"};
  const auto fields = db.describe("person");

  for (const auto &fld : fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryRecordFixture, "Test insert record", "[query][record][insert]") {
  REQUIRE(!db.exists("person"));
  db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute();

  REQUIRE(db.exists("person"));

  auto res = db.query(schema)
    .insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute();

  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select({"id", "name", "age"})
    .from("person")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("id").as<unsigned long>() == 1);
  REQUIRE(row->at("name").as<std::string>() == "hans");
  REQUIRE(row->at("age").as<unsigned short>() == 45);
}

TEST_CASE_METHOD(QueryRecordFixture, "Test update record", "[query][record][update]") {
  REQUIRE(!db.exists("person"));
  db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .execute();

  REQUIRE(db.exists("person"));

  auto res = db.query(schema)
    .insert()
    .into("person", {"id", "name", "age"})
    .values({1, "hans", 45})
    .execute();

  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select({"id", "name", "age"})
    .from("person")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("id").as<unsigned long>() == 1);
  REQUIRE(row->at("name").as<std::string>() == "hans");
  REQUIRE(row->at("age").as<unsigned short>() == 45);

  res = db.query(schema)
    .update("person")
    .set({{"name", "jane"}, {"age", 47}})
    .where("name"_col == "hans")
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select({"id", "name", "age"})
    .from("person")
    .fetch_one();

  REQUIRE(row.has_value());
  REQUIRE(row->at("id").as<unsigned long>() == 1);
  REQUIRE(row->at("name").as<std::string>() == "jane");
  REQUIRE(row->at("age").as<unsigned short>() == 47);
}

TEST_CASE_METHOD(QueryRecordFixture, "Test prepared record statement", "[query][record][prepared]") {
  REQUIRE(!db.exists("person"));
  auto stmt = db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
      make_column<std::string>("name", 255),
      make_column<unsigned short>("age")
    })
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);
  REQUIRE(db.exists("person"));

  const std::vector<std::string> cols = {"id", "name", "age"};
  const auto fields = db.describe("person");

  for (const auto &fld : fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryRecordFixture, "Test scalar result", "[query][record][scalar][result]") {
  REQUIRE(!db.exists("person"));
  db.query(schema).create()
    .table("person", {
      make_pk_column<unsigned long>("id"),
    })
    .execute();

  REQUIRE(db.exists("person"));

  std::vector<unsigned long> ids({ 1,2,3,4 });

  for(auto id : ids) {
    auto res = db.query(schema)
      .insert()
      .into("person", {"id"})
      .values({id})
      .execute();
    REQUIRE(res == 1);
  }

  auto stmt = db.query(schema)
    .select({"id"})
    .from("person")
    .order_by("id"_col).asc()
    .prepare();

  auto rows = stmt.fetch();

  size_t index{0};
  for (const auto &row : rows) {
    REQUIRE(row.at("id").as<unsigned long>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt.reset();

  rows = stmt.fetch();

  index = 0;
  for (const auto &row : rows) {
    REQUIRE(row.at("id").as<unsigned long>() == ids[index]);
    ++index;
  }
  REQUIRE(index == 4);

  stmt.reset();

  auto row = stmt.fetch_one();
  REQUIRE(row.has_value());
  REQUIRE(row->at("id").as<unsigned long>() == ids[0]);
}