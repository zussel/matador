#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include "matador/query/runtime/connection.hpp"
#include "matador/query/dialect/dialect.hpp"

#include "matador/query/criteria.hpp"
#include "matador/query/mapping/generators.hpp"
#include "matador/query/builder/builder.hpp"
#include "matador/query/builder/query.hpp"
#include "matador/query/schema/schema.hpp"
#include "matador/query/meta_table_macro.hpp"

#include "matador/utils/types.hpp"
#include "matador/utils/string.hpp"

#include "models/types.hpp"

#include "fixtures/QueryFixture.hpp"

#include <iostream>

using namespace matador::test;
using namespace matador::query;
using namespace matador::utils;

META_TABLE(
  types,
  TYPE,
  id,
  val_char,
  val_float,
  val_double,
  val_short,
  val_int,
  val_long_long,
  val_unsigned_char,
  val_unsigned_short,
  val_unsigned_int,
  val_unsigned_long_long,
  val_bool,
  val_string,
  val_varchar,
  val_binary
);



TEST_CASE_METHOD(QueryFixture, "Insert and select basic datatypes", "[query][datatypes]") {
  using namespace matador::query::meta;

  REQUIRE(repo.attach<types>("types").is_ok());

  const auto cres = manager.create(db);
  REQUIRE(cres.is_ok());

  float float_value = 2.44557f;
  double double_value = 11111.23433345;
  int8_t cval = 'c';
  short sval = (std::numeric_limits<short>::min)();
  int ival = (std::numeric_limits<int>::min)();
  int64_t llval = (std::numeric_limits<long long>::max)();
  unsigned char ucval = (std::numeric_limits<unsigned char>::max)();
  unsigned short usval = (std::numeric_limits<unsigned short>::max)();
  unsigned int uival = (std::numeric_limits<unsigned int>::max)();
  uint64_t ullval = (std::numeric_limits<unsigned long long>::max)();
  if (db.type() == "sqlite" || db.type() == "postgres") {
    ullval = (std::numeric_limits<long long>::max)();
  }
  bool bval = true;
    // const char *cstr("Armer schwarzer Kater");
    std::string varcharval("hallo welt");
    std::string strval = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam "
            "nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, "
            "sed diam voluptua. At vero eos et accusam et justo duo dolores et ea "
            "rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. "
            "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy "
            "eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. "
            "At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd "
            "gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
    // matador::date date_val(15, 3, 2015);
    // auto time_val = matador::time(2015, 3, 15, 13, 56, 23, 123);
    matador::utils::blob_type_t blob_val {1,2,3,4,5,6,7,8};

    types t {
            1,
            cval, sval, ival, llval,
            ucval, usval, uival, ullval,
            float_value, double_value,
            bval,
            // "Armer schwarzer Kater",
            strval, varcharval,
            // date_val, time_val,
            blob_val
    };

    auto res = insert()
      .into(TYPE(), TYPE().columns())
      .values(t)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);

    auto result = select(TYPE().columns())
      .from("types")
      .fetch_one<types>(db);
    REQUIRE(result.is_ok());
    REQUIRE(result->get() != nullptr);

    REQUIRE((*result)->id_ == 1);
    REQUIRE((*result)->char_ == cval);
    REQUIRE((*result)->short_ == sval);
    REQUIRE((*result)->int_ == ival);
    REQUIRE((*result)->long64_ == llval);
    REQUIRE((*result)->unsigned_char_ == ucval);
    REQUIRE((*result)->unsigned_short_ == usval);
    REQUIRE((*result)->unsigned_int_ == uival);
    REQUIRE((*result)->unsigned_long64_ == ullval);
    REQUIRE((*result)->float_ == float_value);
    REQUIRE((*result)->double_ == double_value);
    // REQUIRE(strcmp((*result)->cstr_, cstr) == 0);
    REQUIRE((*result)->bool_ == bval);
    REQUIRE((*result)->varchar_ == varcharval);
    REQUIRE((*result)->string_ == strval);
    // REQUIRE((*result)->date_ == date_val);
    // REQUIRE((*result)->time_ == time_val);
    REQUIRE((*result)->binary_ == blob_val);
}

TEST_CASE_METHOD( QueryFixture, "Test quoted identifier", "[query][quotes][identifier]" ) {
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
  std::vector<std::string> column_names = { "from", "to"};
  std::vector types = {basic_type::Varchar, basic_type::Varchar};
  const auto columns = db.describe("quotes");
  REQUIRE(columns.is_ok());

  size_t i = 0;
  for (const auto &col : *columns) {
    REQUIRE(col.name() == column_names[i]);
    REQUIRE(col.type() == types[i]);
    ++i;
  }

  res = insert()
    .into("quotes", {"from", "to"})
    .values({"Berlin", "London"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("from").as<std::string>() == "Berlin");
  REQUIRE(row.value()->at("to").as<std::string>() == "London");

  res = update("quotes")
    .set("from", "Hamburg")
    .set("to", "New York")
    .where("from"_col == "Berlin")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({"from", "to"})
    .from("quotes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("from").as<std::string>() == "Hamburg");
  REQUIRE(row.value()->at("to").as<std::string>() == "New York");
}

TEST_CASE_METHOD( QueryFixture, "Test quoted column names", "[query][quotes][column]" ) {
  const auto start_quote = db.dialect().token_at(dialect_token::StartQuote);
  const auto end_quote = db.dialect().token_at(dialect_token::EndQuote);

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
    auto res = create()
      .table("quotes")
      .columns({
          col(name, basic_type::Varchar, 255)
      })
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 0);

    const auto columns = db.describe("quotes");
    REQUIRE(columns.is_ok());

    for (const auto &col : *columns) {
      REQUIRE(col.name() == name);
      REQUIRE(col.type() == basic_type::Varchar);
    }

    res = drop()
      .table("quotes")
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 0);
  }
}

TEST_CASE_METHOD(QueryFixture, "Test quoted literals", "[query][quotes][literals]") {
  auto res = create()
    .table("escapes")
    .columns({
        col("name", basic_type::Varchar, 255)
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("escapes");

  res = insert()
    .into("escapes", {"name"})
    .values({"text"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({"name"})
    .from("escapes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("name").as<std::string>() == "text");

  res = update("escapes")
    .set("name", "text'd")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({"name"})
    .from("escapes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("name").as<std::string>() == "text'd");

  res = update("escapes")
    .set("name", "text\nhello\tworld")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({"name"})
    .from("escapes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("name").as<std::string>() == "text\nhello\tworld");

  res = update("escapes")
    .set("name", "text \"text\"")
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({"name"})
    .from("escapes")
    .fetch_one(db);
  REQUIRE(row.is_ok());

  REQUIRE(row->has_value());
  REQUIRE(row.value()->at("name").as<std::string>() == "text \"text\"");
}

TEST_CASE_METHOD(QueryFixture, "Test describe table", "[query][describe][table]") {
  REQUIRE(repo.attach<types>("types")
    .and_then([this] { return manager.create(db); }));

  const auto columns = db.describe("types");
  REQUIRE(columns.is_ok());

  std::vector<std::string> column_names = { "id",
                                            "val_char", "val_float", "val_double", "val_short",
                                            "val_int", "val_long_long", "val_unsigned_char",
                                            "val_unsigned_short", "val_unsigned_int", "val_unsigned_long_long",
                                            "val_bool", "val_string", "val_varchar",
                                            // "val_date", "val_time",
                                            "val_binary"};
  const std::vector<std::function<bool (const column&)>> type_check = {
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_floating_point(); },
    [](const column &cf) { return cf.is_floating_point(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_integer(); },
    [](const column &cf) { return cf.is_bool(); },
    [](const column &cf) { return cf.is_string(); },
    [](const column &cf) { return cf.is_varchar(); },
    // [](const column_definition &cf) { return cf.is_date(); },
    // [](const column_definition &cf) { return cf.is_time(); },
    [](const column &cf) { return cf.is_blob(); }
  };

  const auto &cols = columns.value();
  size_t i = 0;
  for (const auto &col : cols) {
    REQUIRE(col.name() == column_names[i]);
    REQUIRE(type_check[i](col));
    ++i;
  }
}

TEST_CASE_METHOD(QueryFixture, "Test unknown table", "[query][table]") {
  const auto result = select({"name"})
    .from("person")
    .fetch_all(db);

  REQUIRE(result.is_error());
}

namespace matador::test::temporary {
struct pk {
  template<class Operator>
  void process(Operator &op) {
    access::primary_key(op, "id", id);
    access::attribute(op, "name", name, VarChar255);
  }

  uint32_t id{};
  std::string name;
};
}

// META_TABLE(pks, PK, id, name);

namespace matador::query::meta {
namespace internal {
class pks_table : public typed_query_table<pks_table> {
public:
  pks_table() : typed_query_table<
                  pks_table>("pks", {column::make_plain("id"), column::make_plain("name"),}),
                id(*column_by_name(*this, "id")), name(*column_by_name(*this, "name")) {
  }

  pks_table(const pks_table &other) : typed_query_table<pks_table>(other),
                                      id(*column_by_name(*this, "id")),
                                      name(*column_by_name(*this, "name")) {
  }

  pks_table(pks_table &&other) noexcept : typed_query_table<pks_table>(std::move(other)),
                                          id(*column_by_name(*this, "id")),
                                          name(*column_by_name(*this, "name")) {
  }

  pks_table &operator=(const pks_table &) = delete;
  pks_table &operator=(pks_table &&) = delete;
  const matador::query::column &id;
  const matador::query::column &name;
};
}

inline static const internal::pks_table &PK() {
  static const internal::pks_table table;
  return table;
}
}
TEST_CASE_METHOD(QueryFixture, "Test primary key", "[query][primary key]") {
  using namespace matador::test::temporary;
  using namespace matador::query;
  using namespace matador::query::meta;

  REQUIRE(repo.attach<pk>("pks")
    .and_then([this] { return manager.create(db); }));

  pk pk1{ 7, "george" };

  auto res = insert()
    .into(PK(), PK().columns())
    .values(pk1)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select(PK().columns())
    .from(PK())
    .fetch_one<pk>(db);
  REQUIRE(row.is_ok());
  REQUIRE(row->get() != nullptr);
  REQUIRE(row.value()->id > 0);
}

TEST_CASE_METHOD(QueryFixture, "Test primary key prepared", "[query][primary key][prepared]") {
  using namespace matador::test::temporary;
  using namespace matador::query::meta;

  REQUIRE(repo.attach<pk>("pks")
    .and_then([this] { return manager.create(db); }));

  pk pk1{ 7, "george" };

  auto stmt = insert()
    .into(PK(), PK().columns())
    .values(placeholders<pk>())
    .prepare(db);
  REQUIRE(stmt);

  auto res = stmt->bind(pk1)
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  stmt = select(PK().columns())
    .from(PK())
    .prepare(db);
  REQUIRE(stmt);

  auto row = stmt->fetch_one<pk>();
  REQUIRE(row.is_ok());
  REQUIRE(*row);
  REQUIRE(row.value()->id > 0);
  REQUIRE(row.value()->name == "george");
}

// namespace matador::test::temporary {
// struct appointment
// {
//   unsigned long id{};
//   std::string name;
//   matador::time time_point{};
//   matador::date date_point{};
//
//   template < class Operator >
//   void process(Operator &op)
//   {
//     matador::access::primary_key(op, "id", id);
//     matador::access::attribute(op, "name", name, 255);
//     matador::access::attribute(op, "time_point", time_point);
//     matador::access::attribute(op, "date_point", date_point);
//   }
// };
//
// }
// TEST_CASE_METHOD(QueryFixture, "Test select time and date", "[query][select][time]") {
//   using namespace matador::test::temporary;
//   using namespace matador::sql;
//   REQUIRE(schema.attach<appointment>("appointment"));
//   auto res = create()
//     .table<appointment>("appointment", schema)
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 0);
//   tables_to_drop.emplace("appointment");
//
//   auto dinner = appointment{ 1, "dinner" };
//   auto time_str = matador::utils::to_string(dinner.time_point);
//   auto date_str = matador::utils::to_string(dinner.date_point);
//
//   res = insert()
//     .into("appointment", column_generator::generate<appointment>(schema))
//     .values(dinner)
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 1);
//
//   auto row = select<appointment>(schema)
//     .from("appointment")
//     .fetch_one<appointment>(db);
//   REQUIRE(row.is_ok());
//
//   REQUIRE(*row != nullptr);
//   REQUIRE(matador::utils::to_string(row.value()->time_point) == time_str);
//   REQUIRE(matador::utils::to_string(row.value()->date_point) == date_str);
// }
//
// TEST_CASE_METHOD(QueryFixture, "Test null column", "[query][select][null]") {
//   using namespace matador::sql;
//
//   auto res = create()
//     .table("person", {
//       make_pk_column<unsigned long>("id"),
//       make_column<std::string>("first_name", 255, null_option::NULLABLE),
//       make_column<std::string>("last_name", 255, null_option::NULLABLE)
//     })
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 0);
//   tables_to_drop.emplace("person");
//
//   res = insert()
//     .into("person", {"id", "first_name"})
//     .values({1, "george"})
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 1);
//
//   res = insert()
//     .into("person", {"id", "last_name"})
//     .values({2, "clooney"})
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 1);
//
//   auto result = select({"id", "first_name", "last_name"})
//     .from("person")
//     .fetch_all(db);
//   REQUIRE(result.is_ok());
//
//   std::vector<std::string> expected_first_names{"george", ""};
//   std::vector<std::string> expected_last_names{"", "clooney"};
//   size_t index{0};
//   for (const auto& row : *result) {
//     auto first_name = row.at<std::string>("first_name");
//     auto last_name = row.at<std::string>("last_name");
//     std::cout << "first name " << first_name.value() << " last name " << last_name.value() << std::endl;
//     REQUIRE(first_name == expected_first_names[index]);
//     REQUIRE(last_name == expected_last_names[index++]);
//   }
// }
//
// TEST_CASE_METHOD(QueryFixture, "Test null column prepared", "[query][select][null][prepared]") {
//   using namespace matador::sql;
//
//   auto res = create()
//     .table("person", {
//       make_pk_column<unsigned long>("id"),
//       make_column<std::string>("first_name", 255, null_option::NULLABLE),
//       make_column<std::string>("last_name", 255, null_option::NULLABLE)
//     })
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 0);
//   tables_to_drop.emplace("person");
//
//   res = insert()
//     .into("person", {"id", "first_name"})
//     .values({1, "george"})
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 1);
//
//   res = insert()
//     .into("person", {"id", "last_name"})
//     .values({2, "clooney"})
//     .execute(db);
//   REQUIRE(res.is_ok());
//   REQUIRE(res->affected_rows == 1);
//
//   auto result = select({"id", "first_name", "last_name"})
//     .from("person")
//     .fetch_all(db);
//   REQUIRE(result.is_ok());
//
//   std::vector<std::string> expected_first_names{"george", ""};
//   std::vector<std::string> expected_last_names{"", "clooney"};
//   size_t index{0};
//   for (const auto& row : *result) {
//     auto first_name = row.at("first_name").as<std::string>();
//     auto last_name = row.at("last_name").as<std::string>();
//     REQUIRE(first_name == expected_first_names[index]);
//     REQUIRE(last_name == expected_last_names[index++]);
//   }
// }