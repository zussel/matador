#include <catch2/catch_test_macros.hpp>

#include "matador/query/relations/collection.hpp"
#include "matador/query/mapping/access.hpp"

namespace {
using namespace matador::query;

struct child {
  uint64_t id{};
  template<class Operator>
  void process(Operator &op) { access::primary_key(op, "id", id); }
};

using child_ptr = object_ptr<child>;

class test_resolver : public collection_resolver<child_ptr> {
public:
  test_resolver() : collection_resolver(typeid(child), "children") {}
  std::vector<child_ptr> resolve(const identifier &id) override {
    ++calls;
    last_id = id;
    return items;
  }
  int calls{};
  identifier last_id;
  std::vector<child_ptr> items;
};

child_ptr make_child(uint64_t id, bool persistent = false) {
  auto result = make_object<child>();
  result.primary_key(identifier(id));
  if (persistent) result.change_state(object_state::Persistent);
  return result;
}
}

TEST_CASE("Collection proxy attaches and merges local elements once", "[collection-proxy]") {
  auto local = make_child(1, true);
  auto transient = make_child(0);
  auto transient_with_key = make_child(2);
  auto db_duplicate = make_child(1, true);
  auto db_keyed = make_child(2, true);
  auto db_new = make_child(3, true);
  auto resolver = std::make_shared<test_resolver>();
  resolver->items = {db_duplicate, db_keyed, db_new, db_new};
  collection<child_ptr> values(std::vector<child_ptr>{local, transient, transient_with_key});
  auto copy = values;
  auto proxy = values.proxy();
  proxy->attach(resolver, identifier(uint64_t{42}));
  proxy->attach(resolver, identifier(uint64_t{42}));
  REQUIRE(values.proxy() == proxy);
  REQUIRE(copy.proxy() == proxy);
  REQUIRE(resolver->calls == 0);
  REQUIRE(values.size() == 5);
  REQUIRE(resolver->calls == 1);
  REQUIRE(resolver->last_id == identifier(uint64_t{42}));
  auto it = values.begin();
  REQUIRE(it->proxy() == local.proxy());
  REQUIRE((++it)->proxy() == transient.proxy());
  REQUIRE((++it)->proxy() == transient_with_key.proxy());
  REQUIRE((++it)->proxy() == db_keyed.proxy());
  REQUIRE((++it)->proxy() == db_new.proxy());
  proxy->attach(resolver, identifier(uint64_t{42}));
  REQUIRE(copy.size() == 5);
  REQUIRE(resolver->calls == 1);
}

TEST_CASE("Collection proxy rejects missing resolvers and remains usable", "[collection-proxy]") {
  collection<child_ptr> values(std::vector<child_ptr>{make_child(0)});
  auto proxy = values.proxy();
  REQUIRE_THROWS(proxy->attach({}, identifier(uint64_t{1})));
  REQUIRE(values.size() == 1);
  auto resolver = std::make_shared<test_resolver>();
  REQUIRE_THROWS(proxy->attach(resolver, identifier{}));
  REQUIRE(values.size() == 1);
  proxy->attach(resolver, identifier(uint64_t{1}));
  resolver.reset();
  REQUIRE_THROWS(values.size());
  REQUIRE_THROWS(values.size());
}

TEST_CASE("Already loaded collection proxy is not reset by attach", "[collection-proxy]") {
  auto resolver = std::make_shared<test_resolver>();
  auto item = make_child(1, true);
  auto proxy = std::make_shared<collection_proxy<child_ptr>>(resolver, std::vector<child_ptr>{item});
  proxy->attach(resolver, identifier(uint64_t{7}));
  REQUIRE(proxy->size() == 1);
  REQUIRE(proxy->begin()->proxy() == item.proxy());
  REQUIRE(resolver->calls == 0);
}