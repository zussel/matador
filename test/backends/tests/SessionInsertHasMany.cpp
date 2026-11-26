#include "catch2/catch_test_macros.hpp"

#include "fixtures/SessionFixture.hpp"

#include "connection.hpp"

#include "matador/query/runtime/session.hpp"

#include "models/author.hpp"
#include "models/book.hpp"

using namespace matador;
using namespace matador::query;
using namespace matador::test;

namespace matador::test {
namespace {
template <const primary_key_options &PkAttribute>
struct colorlist_pk_generator {
  unsigned int id{};
  std::string name;
  collection<std::string> colors;

  colorlist_pk_generator() = default;
  colorlist_pk_generator(std::string name, const std::vector<std::string> &color_names)
      : name(std::move(name)), colors(color_names) {}

  template <typename Operator> void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "name", name, VarChar511);
    field::has_many(op, "colors", colors, "colorlist_id", CascadeAllFetchLazy);
  }
};
} // namespace

using colorlist_identity = colorlist_pk_generator<Identity>;
using colorlist_sequence = colorlist_pk_generator<Sequence>;
using colorlist_table = colorlist_pk_generator<Table>;
}

template<typename AuthorType, typename BookType>
static query::object_ptr<AuthorType> create_author() {
  auto s_king = make_object<AuthorType>("Steven King");

  s_king->books.push_back(make_object<BookType>("Carrie", nullobj, 1974));
  s_king->books.push_back(make_object<BookType>("The Shining", nullobj, 1977));
  s_king->books.push_back(make_object<BookType>("It", nullobj, 1986));
  s_king->books.push_back(make_object<BookType>("Misery", nullobj, 1987));
  s_king->books.push_back(make_object<BookType>("The Dark Tower: The Gunslinger", nullobj, 1982));

  return s_king;
}

template<typename AuthorType>
static void validate_author_state(const query::object_ptr<AuthorType>& ptr, object_state expected_state) {
  REQUIRE(ptr.is_state(expected_state));
  for (auto &b: ptr->books) {
    REQUIRE(b.is_state(expected_state));
  }
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many relation", "[session][insert][has_many]") {
  const auto result = schema.attach<book>("books")
    .and_then( [this] { return schema.attach<author>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  auto s_king = make_object<author>(1, "Steven", "King", "21.9.1947", 1956, false);

  s_king->books.push_back(make_object<book>(2, "Carrie", nullobj, 1974));
  s_king->books.push_back(make_object<book>(3, "The Shining", nullobj, 1977));
  s_king->books.push_back(make_object<book>(4, "It", nullobj, 1986));
  s_king->books.push_back(make_object<book>(5, "Misery", nullobj, 1987));
  s_king->books.push_back(make_object<book>(6, "The Dark Tower: The Gunslinger", nullobj, 1982));

  validate_author_state(s_king, object_state::Transient);
  auto res = ses.insert(s_king);
  REQUIRE(res.is_ok());
  validate_author_state(s_king, object_state::Persistent);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many relation with identity", "[session][insert][has_many][identity]") {
  const auto result = schema.attach<book_identity>("books")
    .and_then( [this] { return schema.attach<author_identity>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  auto s_king = create_author<author_identity, book_identity>();

  validate_author_state(s_king, object_state::Transient);
  auto res = ses.insert(s_king);
  REQUIRE(res.is_ok());
  validate_author_state(s_king, object_state::Persistent);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many relation with sequence", "[session][insert][has_many][sequence]") {
  const auto result = schema.attach<book_sequence>("books")
    .and_then( [this] { return schema.attach<author_sequence>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  auto s_king = create_author<author_sequence, book_sequence>();

  validate_author_state(s_king, object_state::Transient);
  auto res = ses.insert(s_king);
  REQUIRE(res.is_ok());
  validate_author_state(s_king, object_state::Persistent);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many relation with table sequence", "[session][insert][has_many][table_sequence]") {
  const auto result = schema.attach<book_table>("books")
    .and_then( [this] { return schema.attach<author_table>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  auto s_king = create_author<author_table, book_table>();

  validate_author_state(s_king, object_state::Transient);
  auto res = ses.insert(s_king);
  REQUIRE(res.is_ok());
  validate_author_state(s_king, object_state::Persistent);

  auto author_result = ses.find<author_table>(s_king->id);
  REQUIRE(author_result);
  auto author = *author_result;
  REQUIRE(author.is_persistent());
  REQUIRE(author->books.size() == 5);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many primitive relation with identity", "[session][insert][has_many][primitive][identity]") {
  const auto result = schema.attach<colorlist_identity>("colorlist")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  const auto c = make_object<colorlist_identity>("rgb", std::vector<std::string>{"red", "green", "blue"});

  auto res = ses.insert(c);
  REQUIRE(res.is_ok());

  auto colorlist_result = ses.find<colorlist_identity>(c->id);
  REQUIRE(colorlist_result);
  auto clist = *colorlist_result;
  REQUIRE(clist.is_persistent());
  REQUIRE(clist->colors.size() == 3);
  std::vector<std::string> expected_colors{"red", "green", "blue"};
  for (const auto &cstr: clist->colors) {
    REQUIRE(std::find(expected_colors.begin(), expected_colors.end(), cstr) != expected_colors.end());
  }
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many primitive relation with table sequence", "[session][insert][has_many][primitive][table_sequence]") {
  const auto result = schema.attach<colorlist_table>("colorlist")
    .and_then([this] { return manager.create(db); } );
  if (!result.is_ok()) WARN(result.err().message() << ": " << result.err().error_info("sql").value_or("") << " " << result.err().error_info("message").value_or(""));
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  const auto c = make_object<colorlist_table>("rgb", std::vector<std::string>{"red", "green", "blue"});

  auto res = ses.insert(c);
  REQUIRE(res.is_ok());

  auto colorlist_result = ses.find<colorlist_table>(c->id);
  REQUIRE(colorlist_result);
  auto clist = *colorlist_result;
  REQUIRE(clist.is_persistent());
  REQUIRE(clist->colors.size() == 3);
  std::vector<std::string> expected_colors{"red", "green", "blue"};
  for (const auto &cstr: clist->colors) {
    REQUIRE(std::find(expected_colors.begin(), expected_colors.end(), cstr) != expected_colors.end());
  }
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many primitive relation with sequence", "[session][insert][has_many][primitive][sequence]") {
  const auto result = schema.attach<colorlist_sequence>("colorlist")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  const auto c = make_object<colorlist_sequence>("rgb", std::vector<std::string>{"red", "green", "blue"});

  auto res = ses.insert(c);
  REQUIRE(res.is_ok());

  auto colorlist_result = ses.find<colorlist_sequence>(c->id);
  REQUIRE(colorlist_result);
  auto clist = *colorlist_result;
  REQUIRE(clist.is_persistent());
  REQUIRE(clist->colors.size() == 3);
  std::vector<std::string> expected_colors{"red", "green", "blue"};
  for (const auto &cstr: clist->colors) {
    REQUIRE(std::find(expected_colors.begin(), expected_colors.end(), cstr) != expected_colors.end());
  }
}
