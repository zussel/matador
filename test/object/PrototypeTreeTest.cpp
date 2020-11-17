#include "PrototypeTreeTest.hpp"

#include "matador/object/object_store.hpp"
#include "../datatypes.hpp"
#include "../entities.hpp"

#include <fstream>

using namespace matador;
using namespace std;

PrototypeTreeTestUnit::PrototypeTreeTestUnit()
  : unit_test("tree", "Prototype Tree Test Unit")
{
  add_test("empty", std::bind(&PrototypeTreeTestUnit::test_empty, this), "test empty prototype tree");
  add_test("insert", std::bind(&PrototypeTreeTestUnit::test_insert, this), "test insert element by template arguments");
  add_test("find", std::bind(&PrototypeTreeTestUnit::test_find, this), "test find element");
  add_test("remove", std::bind(&PrototypeTreeTestUnit::test_remove, this), "test remove element");
  add_test("erase", std::bind(&PrototypeTreeTestUnit::test_erase, this), "test erase element");
  add_test("clear", std::bind(&PrototypeTreeTestUnit::test_clear, this), "test clear prototype tree");
  add_test("has_many", std::bind(&PrototypeTreeTestUnit::test_has_many, this), "test insert has_many relation");
  add_test("decrement", std::bind(&PrototypeTreeTestUnit::test_decrement, this), "test decrement iterator");
  add_test("count", std::bind(&PrototypeTreeTestUnit::test_count, this), "test count of prototypes");
  add_test("child_of", std::bind(&PrototypeTreeTestUnit::test_child_of, this), "test child of element");
  add_test("traverse", std::bind(&PrototypeTreeTestUnit::test_traverse, this), "test traversing the prototype tree");
  add_test("const_traverse", std::bind(&PrototypeTreeTestUnit::test_const_traverse, this), "test const traversing the prototype tree");
}

void PrototypeTreeTestUnit::test_empty()
{
  object_store ptree;
  UNIT_ASSERT_TRUE(ptree.empty());
  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0);
}

void PrototypeTreeTestUnit::test_insert()
{
  object_store ptree;
  UNIT_ASSERT_EXCEPTION(ptree.attach<datatypes>("item", matador::object_store::not_abstract, "baba"), object_exception, "unknown prototype type");

  ptree.attach<datatypes>("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1);

  UNIT_ASSERT_EXCEPTION(ptree.attach<datatypes>("item"), object_exception, "prototype already inserted: item");

  ptree.attach<ItemA, datatypes>("item_a");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)2);
}

void PrototypeTreeTestUnit::test_find()
{
  object_store ptree;
  ptree.attach<datatypes>("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1);

  prototype_iterator elem = ptree.find("item");

  UNIT_ASSERT_TRUE(elem != ptree.end());
  UNIT_ASSERT_EQUAL(elem->type(), "item");

  elem = ptree.find<datatypes>();

  UNIT_ASSERT_TRUE(elem != ptree.end());
  UNIT_ASSERT_EQUAL(elem->type(), "item");

  elem = ptree.find("unknown");
  UNIT_ASSERT_TRUE(elem == ptree.end());

  ptree.attach<ObjectItem<datatypes>>("object_item");
  elem = ptree.find("object_item");

  UNIT_ASSERT_TRUE(elem != ptree.end());
  UNIT_ASSERT_EQUAL(elem->type(), "object_item");

  elem = ptree.find("item");

  UNIT_ASSERT_TRUE(elem != ptree.end());
  UNIT_ASSERT_EQUAL(elem->type(), "item");

//  ptree.insert(new list_object_producer<ItemPtrList>("ptr_list"));
}

void PrototypeTreeTestUnit::test_remove()
{
  object_store ptree;
  ptree.attach<datatypes>("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1);

  UNIT_ASSERT_EXCEPTION(ptree.detach(nullptr), object_exception, "invalid type (null)");
  UNIT_ASSERT_EXCEPTION(ptree.detach("ITEM"), object_exception, "unknown prototype type");

  ptree.detach("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0);
  UNIT_ASSERT_TRUE(ptree.empty());
}

void PrototypeTreeTestUnit::test_erase()
{
  object_store ptree;
  prototype_iterator iter = ptree.attach<datatypes>("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1);

  prototype_iterator iter2;
  UNIT_ASSERT_EXCEPTION(ptree.detach(iter2), object_exception, "invalid prototype iterator");

  ptree.detach(iter);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0);
  UNIT_ASSERT_TRUE(ptree.empty());
}

void PrototypeTreeTestUnit::test_clear()
{
  object_store ptree;
  ptree.attach<datatypes>("item");
  ptree.attach<ItemA>("item_a", matador::object_store::not_abstract, "item");
  ptree.attach<ItemB>("item_b", matador::object_store::not_abstract, "item");
  ptree.attach<ItemC>("item_c", matador::object_store::not_abstract, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)4);

  ptree.clear(true);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0);
  UNIT_ASSERT_TRUE(ptree.empty());
}

void PrototypeTreeTestUnit::test_has_many()
{
  object_store ptree;
  ptree.attach<book>("book");
  ptree.attach<book_list>("book_list");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)3);

  prototype_iterator i = ptree.find("books");

  UNIT_ASSERT_TRUE(i != ptree.end());
}

void PrototypeTreeTestUnit::test_decrement()
{
  object_store ptree;
  ptree.attach<datatypes>("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1);

  prototype_iterator i = ptree.end();

  --i;

  UNIT_ASSERT_TRUE(i == ptree.begin());
}

void PrototypeTreeTestUnit::test_count() {
  object_store ptree;
  ptree.attach<datatypes>("item");
  ptree.attach<ItemA>("item_a", matador::object_store::not_abstract, "item");
  ptree.attach<ItemB>("item_b", matador::object_store::not_abstract, "item");
  ptree.attach<ItemC>("item_c", matador::object_store::not_abstract, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)4);
}

void PrototypeTreeTestUnit::test_child_of()
{
  object_store ptree;
  ptree.attach<datatypes>("item");
  ptree.attach<ItemA>("item_a", matador::object_store::not_abstract, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)2);

  prototype_iterator root = ptree.begin();

  prototype_iterator item_a = ptree.find<ItemA>();

  UNIT_ASSERT_FALSE(root->is_child_of(item_a.get()));

  UNIT_ASSERT_TRUE(item_a->is_child_of(root.get()));
}

void PrototypeTreeTestUnit::test_traverse()
{
  object_store ptree;
  ptree.attach<datatypes>("item");
  ptree.attach<ItemA>("item_a", matador::object_store::not_abstract, "item");
  ptree.attach<ItemB>("item_b", matador::object_store::not_abstract, "item");
  ptree.attach<ItemC>("item_c", matador::object_store::not_abstract, "item");

  prototype_iterator first;
  first = ptree.begin();
  prototype_iterator last = ptree.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4);
    ++first;
    ++count;
  }

  UNIT_ASSERT_EQUAL(count, 4);

  first = ptree.begin();
  ++first;
  first--;

  UNIT_ASSERT_TRUE(first == ptree.begin());
}

void PrototypeTreeTestUnit::test_const_traverse()
{
  object_store ptree;
  ptree.attach<datatypes>("item");
  ptree.attach<ItemA>("item_a", matador::object_store::not_abstract, "item");
  ptree.attach<ItemB>("item_b", matador::object_store::not_abstract, "item");
  ptree.attach<ItemC>("item_c", matador::object_store::not_abstract, "item");

  const_prototype_iterator first= ptree.begin();
  const_prototype_iterator last = ptree.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4);
    ++first;
    ++count;
  }

  UNIT_ASSERT_EQUAL(count, 4);

  first = ptree.begin();
  first++;
  UNIT_ASSERT_EQUAL(first->type(), "item_a");
  UNIT_ASSERT_EQUAL((*first).type(), "item_a");
  UNIT_ASSERT_EQUAL(first.get()->type(), "item_a");
  first--;

  UNIT_ASSERT_TRUE(first == ptree.begin());
}
