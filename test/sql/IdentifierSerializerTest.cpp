#include "IdentifierSerializerTest.h"

#include "matador/sql/connection.hpp"
#include "matador/sql/identifier_binder.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/condition.hpp"

using namespace matador;

template < class IdentifierType, size_t Size = 0 >
struct identifier_entity
{
  IdentifierType id{};
  std::string name;

  template<class Operator>
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
  }

};

template < size_t Size >
struct identifier_entity<std::string, Size>
{
  std::string id{};
  std::string name;

  template<class Operator>
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id, Size);
    matador::access::attribute(op, "name", name, 255);
  }

};

template<typename IdType, typename SecondIdType, typename... OtherIdTypes>
void IdentifierSerializerTest::test_identifier_result(matador::connection &conn)
{
  test_identifier_result<IdType>(conn);
  test_identifier_result<SecondIdType, OtherIdTypes...>(conn);
}

template<typename IdType>
void IdentifierSerializerTest::test_identifier_result(matador::connection &conn)
{
  query<identifier_entity<IdType>> q{};

  auto res = q.create("id_type").execute(conn);

  identifier_entity<IdType> id { 1, "george" };

  res = q.insert(id).into("id_type").execute(conn);
  res = q.select().from("id_type").execute(conn);

  for (const auto &i : res) {
    UNIT_EXPECT_EQUAL(i->id, static_cast<IdType>(1));
    UNIT_EXPECT_EQUAL(i->name, "george");
  }

  res = q.drop("id_type").execute(conn);
}

template<>
void IdentifierSerializerTest::test_identifier_result<std::string>(matador::connection &conn)
{
  query<identifier_entity<std::string, 255>> q{};

  auto res = q.create("id_type").execute(conn);

  identifier_entity<std::string, 255> id { "id", "george" };

  res = q.insert(id).execute(conn);
  res = q.select().execute(conn);

  for (const auto &i : res) {
    UNIT_EXPECT_EQUAL(i->id, "id");
    UNIT_EXPECT_EQUAL(i->name, "george");
  }

  res = q.drop("id_type").execute(conn);
}

template<typename IdType, typename SecondIdType, typename... OtherIdTypes>
void IdentifierSerializerTest::test_identifier_row_result(connection &conn)
{
  test_identifier_row_result<IdType>(conn);
  test_identifier_row_result<SecondIdType, OtherIdTypes...>(conn);
}

template<typename IdType>
void IdentifierSerializerTest::test_identifier_row_result(connection &conn)
{
  auto q = query<>();

  auto res = q.create("id_row_type", {
                      make_pk_column<IdType>("id"),
                      make_column<std::string>("name", 255)
                      }).execute(conn);

  res = q.insert({"id", "name"}).into("id_row_type").values({static_cast<IdType>(1), "george"}).execute(conn);

  res = q.select({"id", "name"}).from("id_row_type").execute(conn);

  for (const auto &i : res) {
    UNIT_EXPECT_EQUAL(i->template at<IdType>("id"), static_cast<IdType>(1));
    UNIT_EXPECT_EQUAL(i->template at<std::string>("name"), "george");
  }

  res = q.drop("id_row_type").execute(conn);
}

template<>
void IdentifierSerializerTest::test_identifier_row_result<std::string>(connection &conn)
{
  auto q = query<>();

  auto res = q.create("id_row_type", {
                      make_pk_column<std::string>("id"),
                      make_column<std::string>("name", 255)
                      }).execute(conn);

  res = q.insert({"id", "name"}).into("id_row_type").values({"id", "george"}).execute(conn);

  res = q.select({"id", "name"}).from("id_row_type").execute(conn);

  for (const auto &i : res) {
    UNIT_EXPECT_EQUAL(i->at<std::string>("id"), "id");
    UNIT_EXPECT_EQUAL(i->at<std::string>("name"), "george");
  }

  res = q.drop("id_row_type").execute(conn);
}

template<typename IdType, typename SecondIdType, typename... OtherIdTypes>
void IdentifierSerializerTest::test_identifier_statement_result(connection &conn) {
  test_identifier_statement_result<IdType>(conn);
  test_identifier_statement_result<SecondIdType, OtherIdTypes...>(conn);
}

template<typename IdType>
void IdentifierSerializerTest::test_identifier_statement_result(connection &conn)
{
  identifier_entity<IdType> id { 7, "jane"};
  query<identifier_entity<IdType>> q{};
  auto res = q.create("id_stmt_entity").execute(conn);

  res = q.insert(id).execute(conn);

  id.name = "john";

  auto stmt = q.update("id_stmt_entity").where("id"_col == 7).prepare(conn);
  stmt.bind(0, &id);

  detail::identifier_binder<identifier_entity<IdType>> pk_binder;
  auto pk = identifier{id.id};
  pk_binder.bind(&id, &stmt, 2, pk);

  res = stmt.execute();

  res = q.select().from("id_stmt_entity").where("id"_col == 7).execute(conn);

  for (const auto &p : res) {
    UNIT_EXPECT_EQUAL(p->id, static_cast<IdType>(7));
    UNIT_EXPECT_EQUAL(p->name, "john");
  }

  res = q.drop("id_stmt_entity").execute(conn);
}

IdentifierSerializerTest::IdentifierSerializerTest(const std::string &prefix, std::string dns)
: unit_test(prefix + "_identifier_serializer", "identifier serializer test")
, dns_(std::move(dns))
{
  add_test("result", [this] { test_identifier_result_test(); }, "test identifier result binding");
  add_test("row_result", [this] { test_identifier_row_result_test(); }, "test identifier row result binding");
  add_test("statement_binding", [this] { test_identifier_statement_test(); }, "test identifier statement binding");
}

void IdentifierSerializerTest::test_identifier_result_test()
{
  connection conn(dns_);

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  test_identifier_result<short,
                         int,
                         long,
                         long long,
                         unsigned short,
                         unsigned int,
                         unsigned long,
                         unsigned long long,
                         std::string
                         >(conn);

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void IdentifierSerializerTest::test_identifier_row_result_test()
{
  connection conn(dns_);

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  test_identifier_row_result<
                             short,
                             int,
                             long,
                             long long,
                             unsigned short,
                             unsigned int,
                             unsigned long,
                             unsigned long long
//                             std::string
                             >(conn);

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void IdentifierSerializerTest::test_identifier_statement_test()
{
  connection conn(dns_);

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  test_identifier_statement_result<
                             short,
                             int,
                             long,
                             long long,
                             unsigned short,
                             unsigned int,
                             unsigned long,
                             unsigned long long
//                             std::string
                            >(conn);

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}
