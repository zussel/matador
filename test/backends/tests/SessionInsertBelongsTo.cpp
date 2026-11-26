#include "catch2/catch_test_macros.hpp"

#include "fixtures/SessionFixture.hpp"

#include "connection.hpp"

#include "matador/query/runtime/session.hpp"
#include "matador/query/internal/collection_resolver_attacher.hpp"

#include "models/author.hpp"
#include "models/book.hpp"

using namespace matador::test;
using namespace matador::query;

TEST_CASE("Missing collection resolver fails attachment", "[session][insert][belongs_to][resolver]") {
  author_identity owner;
  auto proxy = owner.books.proxy();
  resolver_service service;
  collection_resolver_attacher attacher;
  auto result = attacher.attach(owner, identifier{1u}, service);
  REQUIRE_FALSE(result.is_ok());
  REQUIRE(owner.books.proxy() == proxy);
  REQUIRE(owner.books.empty());
}

template<typename Author, typename Book>
static void check_collection(session& ses, const object_ptr<Author>& author, const object_ptr<Book>& book) {
  auto found = ses.find<Author>(author->id);
  REQUIRE(found);
  REQUIRE(found.value().proxy() == author.proxy());
  REQUIRE(author->books.size() == 1);
  REQUIRE(found.value()->books.size() == 1);
  auto it = author->books.begin();
  REQUIRE(it != author->books.end());
  REQUIRE((*it)->title == book->title);
  REQUIRE(++it == author->books.end());
  REQUIRE(author->books.size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with belongs to relation with identity", "[session][insert][belongs_to][identity]") {
  const auto result = schema.attach<book_identity>("books")
    .and_then( [this] { return schema.attach<author_identity>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);

  auto s_king = make_object<author_identity>("Steven King");

  const auto carrie = make_object<book_identity>("Carrie", s_king, 1974);

  REQUIRE(carrie.is_transient());
  REQUIRE(ses.insert(carrie).is_ok());
  REQUIRE(carrie.is_persistent());

  check_collection(ses, s_king, carrie);

  auto found_author = ses.find<author_identity>(s_king->id);
  REQUIRE(found_author);
  REQUIRE(found_author.value()->name == "Steven King");
  REQUIRE(found_author.value()->books.size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with belongs to relation with table sequence", "[session][insert][belongs_to][table_sequence]") {
  const auto result = schema.attach<book_table>("books")
    .and_then( [this] { return schema.attach<author_table>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);

  auto s_king = make_object<author_table>("Steven King");

  const auto carrie = make_object<book_table>("Carrie", s_king, 1974);

  REQUIRE(carrie.is_transient());
  REQUIRE(ses.insert(carrie).is_ok());
  REQUIRE(carrie.is_persistent());

  check_collection(ses, s_king, carrie);

  auto found_author = ses.find<author_table>(s_king->id);
  REQUIRE(found_author);
  REQUIRE(found_author.value()->name == "Steven King");
  REQUIRE(found_author.value()->books.size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with belongs to relation with sequence", "[session][insert][belongs_to][sequence]") {
  const auto result = schema.attach<book_sequence>("books")
    .and_then( [this] { return schema.attach<author_sequence>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);

  auto s_king = make_object<author_sequence>("Steven King");

  const auto carrie = make_object<book_sequence>("Carrie", s_king, 1974);

  REQUIRE(carrie.is_transient());
  REQUIRE(ses.insert(carrie).is_ok());
  REQUIRE(carrie.is_persistent());

  check_collection(ses, s_king, carrie);

  auto found_author = ses.find<author_sequence>(s_king->id);
  REQUIRE(found_author);
  REQUIRE(found_author.value()->name == "Steven King");
  REQUIRE(found_author.value()->books.size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with belongs to relation with manual key", "[session][insert][belongs_to][manual]") {
  const auto result = schema.attach<book>("books")
    .and_then([this] { return schema.attach<author>("authors"); })
    .and_then([this] { return manager.create(db); });
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);
  auto s_king = make_object<author>(1, "Steven", "King", "21.9.1947", 1947, false);
  auto carrie = make_object<book>(2, "Carrie", s_king, 1974);
  REQUIRE(ses.insert(carrie).is_ok());
  check_collection(ses, s_king, carrie);
}

TEST_CASE_METHOD(SessionFixture, "Transaction commit attaches inserted collections", "[session][insert][transaction][commit]") {
  const auto result = schema.attach<book_identity>("books")
    .and_then([this] { return schema.attach<author_identity>("authors"); })
    .and_then([this] { return manager.create(db); });
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);
  auto owner = make_object<author_identity>("Steven King");
  auto child = make_object<book_identity>("Carrie", owner, 1974);
  auto proxy = owner->books.proxy();
  auto tx = ses.begin();
  REQUIRE(ses.insert(child).is_ok());
  REQUIRE(owner.is_transient());
  REQUIRE(tx.commit().is_ok());
  REQUIRE(owner->books.proxy() == proxy);
  check_collection(ses, owner, child);
}

TEST_CASE_METHOD(SessionFixture, "Transaction rollback leaves collections unattached", "[session][insert][transaction][rollback]") {
  const auto result = schema.attach<book_identity>("books")
    .and_then([this] { return schema.attach<author_identity>("authors"); })
    .and_then([this] { return manager.create(db); });
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);
  auto owner = make_object<author_identity>("Steven King");
  auto child = make_object<book_identity>("Carrie", owner, 1974);
  auto proxy = owner->books.proxy();
  auto tx = ses.begin();
  REQUIRE(ses.insert(child).is_ok());
  REQUIRE(tx.rollback().is_ok());
  REQUIRE(owner.is_transient());
  REQUIRE(child.is_transient());
  REQUIRE(owner->books.proxy() == proxy);
  REQUIRE(owner->books.empty());
  REQUIRE_FALSE(proxy->owner_id().is_valid());
}

TEST_CASE_METHOD(SessionFixture, "Insert merges prefilled collections with database results", "[session][insert][collection-merge]") {
  const auto result = schema.attach<book_identity>("books")
    .and_then([this] { return schema.attach<author_identity>("authors"); })
    .and_then([this] { return manager.create(db); });
  REQUIRE(result.is_ok());

  session ses({bus, matador::test::connection::dns, 4}, schema);
  auto owner = make_object<author_identity>("Steven King");
  auto child = make_object<book_identity>("Carrie", owner, 1974);
  owner->books.push_back(child);
  auto proxy = owner->books.proxy();
  auto tx = ses.begin();
  REQUIRE(ses.insert(child).is_ok());
  REQUIRE(tx.commit().is_ok());
  REQUIRE(owner->books.proxy() == proxy);
  check_collection(ses, owner, child);
  REQUIRE(owner->books.begin()->proxy() == child.proxy());
}

