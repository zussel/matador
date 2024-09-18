#include "catch2/catch_test_macros.hpp"

#include "SessionFixture.hpp"

#include "models/airplane.hpp"
#include "models/author.hpp"
#include "models/book.hpp"
#include "models/flight.hpp"

using namespace matador;
using namespace matador::test;

TEST_CASE_METHOD(SessionFixture, "Session relation test", "[session][relation]") {
  ses.attach<airplane>("airplanes");
  ses.attach<flight>("flights");
  ses.create_schema();

  tables_to_drop.emplace("airplanes");
  tables_to_drop.emplace("flights");

  auto plane = ses.insert<airplane>(1, "Boeing", "A380");
  auto f = ses.insert<flight>(2, plane, "sully");

  const auto result = ses.find<flight>(2);
  REQUIRE(result.is_ok());
  REQUIRE(result->get()->id == f->id);
  REQUIRE(result->get()->pilot_name == f->pilot_name);
  REQUIRE(result->get()->airplane);
  REQUIRE(result->get()->airplane->id == plane->id);
  REQUIRE(result->get()->airplane->brand == plane->brand);
  REQUIRE(result->get()->airplane->model == plane->model);
}

TEST_CASE_METHOD(SessionFixture, "Use session to find object with id", "[session][find]") {
  ses.attach<airplane>("airplanes");
  ses.create_schema();

  tables_to_drop.emplace("airplanes");

  auto a380 = ses.insert<airplane>(1, "Boeing", "A380");

  auto result = ses.find<airplane>(2);
  REQUIRE(!result.is_ok());
  REQUIRE((result.err() == sql::session_error::FailedToFindObject));

  result = ses.find<airplane>(1);

  REQUIRE(result);
  auto read_a380 = result.value();
  REQUIRE(a380->id == read_a380->id);
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects", "[session][find]") {
  ses.attach<airplane>("airplanes");
  ses.create_schema();

  tables_to_drop.emplace("airplanes");

  std::vector<std::unique_ptr<airplane>> planes;
  planes.emplace_back(new airplane(1, "Airbus", "A380"));
  planes.emplace_back(new airplane(2, "Boeing", "707"));
  planes.emplace_back(new airplane(3, "Boeing", "747"));

  for (auto &&plane: planes) {
    ses.insert(plane.release());
  }

  auto result = ses.find<airplane>();

  std::vector<std::tuple<unsigned long, std::string, std::string>> expected_result {
    {1, "Airbus", "A380"},
    {2, "Boeing", "707"},
    {3, "Boeing", "747"}
  };
  REQUIRE(result);
  auto all_planes = result.release();
  size_t index {0};
  for (const auto &i: all_planes) {
    REQUIRE(i.id == std::get<0>(expected_result[index]));
    REQUIRE(i.brand == std::get<1>(expected_result[index]));
    REQUIRE(i.model == std::get<2>(expected_result[index]));
    ++index;
  }
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects with one-to-many relation", "[session][find][one-to-many]") {
    ses.attach<author>("authors");
    ses.attach<book>("books");
    ses.create_schema();

    std::vector<std::unique_ptr<author>> authors;
    authors.emplace_back(new author{1, "Michael", "Crichton", "21.5.1975", 1975, true, {}});
    authors.emplace_back(new author{ 2, "Steven", "King", "21.5.1956", 1956, false, {}});

    for (auto &&a: authors) {
        ses.insert(a.release());
    }

    auto result = ses.find<author>();
    REQUIRE(result);
    auto all_authors = result.release();
    std::vector<author> author_repo;
    for (const auto &i: all_authors) {
        std::cout << "author: " << i.first_name << std::endl;
        author_repo.push_back(i);
    }

    std::vector<std::unique_ptr<book>> books;
    books.emplace_back( new book(3, ) )
    tables_to_drop.emplace("books");
    tables_to_drop.emplace("authors");
}