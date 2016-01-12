#include "PrototypeTreeTest.hpp"

#include "object/object_store.hpp"
#include "../Item.hpp"

#include <iostream>

using namespace oos;
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
  add_test("relations", std::bind(&PrototypeTreeTestUnit::test_relations, this), "test relations");
}

PrototypeTreeTestUnit::~PrototypeTreeTestUnit()
{}

void PrototypeTreeTestUnit::initialize()
{}

void PrototypeTreeTestUnit::finalize()
{}

void PrototypeTreeTestUnit::test_empty()
{
  object_store ptree;
  UNIT_ASSERT_TRUE(ptree.empty(), "prototype must be empty");
  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0, "prototype size must be zero (0)");
}

void PrototypeTreeTestUnit::test_insert()
{
  object_store ptree;
  UNIT_ASSERT_EXCEPTION(ptree.attach<Item>("item", false, "baba"), object_exception, "unknown prototype type", "inserted with invalid parent");

  ptree.attach<Item>("item", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1, "prototype size must be one (1)");

  UNIT_ASSERT_EXCEPTION(ptree.attach<Item>("item", false), object_exception, "prototype already inserted: item", "inserted same prototype twice");

  ptree.attach<ItemA, Item>("item_a", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)2, "prototype size must be one (2)");
}

void PrototypeTreeTestUnit::test_find()
{
  object_store ptree;
  ptree.attach<Item>("item", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1, "prototype size must be one (1)");

  prototype_iterator elem = ptree.find("item");

  UNIT_ASSERT_TRUE(elem != ptree.end(), "couldn't find prototype");
  UNIT_ASSERT_EQUAL(elem->type(), "item", "type must be 'item'");

  elem = ptree.find<Item>();

  UNIT_ASSERT_TRUE(elem != ptree.end(), "couldn't find prototype");
  UNIT_ASSERT_EQUAL(elem->type(), "item", "type must be 'item'");

  elem = ptree.find("unknown");
  UNIT_ASSERT_TRUE(elem == ptree.end(), "shouldn't find a prototype");

  ptree.attach<ObjectItem<Item>>("object_item", false, "item");
  elem = ptree.find("object_item");

  UNIT_ASSERT_TRUE(elem != ptree.end(), "couldn't find prototype");
  UNIT_ASSERT_EQUAL(elem->type(), "object_item", "type must be 'item'");

  elem = ptree.find("item");

  UNIT_ASSERT_TRUE(elem != ptree.end(), "couldn't find prototype");
  UNIT_ASSERT_EQUAL(elem->type(), "item", "type must be 'item'");

//  ptree.insert(new list_object_producer<ItemPtrList>("ptr_list"), "item_ptr_list");
}

void PrototypeTreeTestUnit::test_remove()
{
  object_store ptree;
  ptree.attach<Item>("item", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1, "prototype size must be one (1)");

  UNIT_ASSERT_EXCEPTION(ptree.detach(0), object_exception, "invalid type (null)", "expect an serializable exception when trying to remove unknown type");
  UNIT_ASSERT_EXCEPTION(ptree.detach("ITEM"), object_exception, "unknown prototype type", "expect an serializable exception when trying to remove unknown type");

  ptree.detach("item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0, "prototype size must be one (0)");
  UNIT_ASSERT_TRUE(ptree.empty(), "prototype tree must be empty");
}

void PrototypeTreeTestUnit::test_erase()
{
  object_store ptree;
  prototype_iterator iter = ptree.attach<Item>("item", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1, "prototype size must be one (1)");

  prototype_iterator iter2;
  UNIT_ASSERT_EXCEPTION(ptree.detach(iter2), object_exception, "invalid prototype iterator", "expect an serializable exception when trying to remove unknown type");

  ptree.detach(iter);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0, "prototype size must be one (0)");
  UNIT_ASSERT_TRUE(ptree.empty(), "prototype tree must be empty");
}

void PrototypeTreeTestUnit::test_clear()
{
  object_store ptree;
  ptree.attach<Item>("item", false);
  ptree.attach<ItemA>("item_a", false, "item");
  ptree.attach<ItemB>("item_b", false, "item");
  ptree.attach<ItemC>("item_c", false, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)4, "prototype size must be one (4)");

  ptree.clear(true);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)0, "prototype size must be one (0)");
  UNIT_ASSERT_TRUE(ptree.empty(), "prototype tree must be empty");
}

void PrototypeTreeTestUnit::test_has_many()
{
  object_store ptree;
  ptree.attach<book>("book", false);
  ptree.attach<book_list>("book_list", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)3, "prototype size must be one (3)");

  prototype_iterator i = ptree.find("books");

  UNIT_ASSERT_TRUE(i != ptree.end(), "should've found 'books' node");
}

void PrototypeTreeTestUnit::test_decrement()
{
  object_store ptree;
  ptree.attach<Item>("item", false);

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)1, "prototype size must be one (1)");

  prototype_iterator i = ptree.end();

  --i;

  UNIT_ASSERT_TRUE(i == ptree.begin(), "iterator must be begin");
}

void PrototypeTreeTestUnit::test_count() {
  object_store ptree;
  ptree.attach<Item>("item", false);
  ptree.attach<ItemA>("item_a", false, "item");
  ptree.attach<ItemB>("item_b", false, "item");
  ptree.attach<ItemC>("item_c", false, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)4, "prototype size must be one (5)");
}

void PrototypeTreeTestUnit::test_child_of()
{
  object_store ptree;
  ptree.attach<Item>("item", false);
  ptree.attach<ItemA>("item_a", false, "item");

  UNIT_ASSERT_EQUAL(ptree.size(), (size_t)2, "prototype size must be one (1)");

  prototype_iterator root = ptree.begin();

  prototype_iterator item_a = ptree.find<ItemA>();

  UNIT_ASSERT_FALSE(root->is_child_of(item_a.get()), "root must not be child of node");

  UNIT_ASSERT_TRUE(item_a->is_child_of(root.get()), "node must be child of root");
}

void PrototypeTreeTestUnit::test_traverse()
{
  object_store ptree;
  ptree.attach<Item>("item", false);
  ptree.attach<ItemA>("item_a", false, "item");
  ptree.attach<ItemB>("item_b", false, "item");
  ptree.attach<ItemC>("item_c", false, "item");

  prototype_iterator first;
  first = ptree.begin();
  prototype_iterator last = ptree.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4, "prototype count isn't valid");
    ++first;
    ++count;
  }

  UNIT_ASSERT_EQUAL(count, 4, "expected prototype size isn't 5");

  first = ptree.begin();
  ++first;
  first--;

  UNIT_ASSERT_TRUE(first == ptree.begin(), "expected prototype iterator to be begin()");
}

void PrototypeTreeTestUnit::test_const_traverse()
{
  object_store ptree;
  ptree.attach<Item>("item", false);
  ptree.attach<ItemA>("item_a", false, "item");
  ptree.attach<ItemB>("item_b", false, "item");
  ptree.attach<ItemC>("item_c", false, "item");

  const_prototype_iterator first= ptree.begin();
  const_prototype_iterator last = ptree.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4, "prototype count isn't valid");
    ++first;
    ++count;
  }

  UNIT_ASSERT_EQUAL(count, 4, "expected prototype size isn't 5");

  first = ptree.begin();
  first++;
  UNIT_ASSERT_EQUAL(first->type(), "item_a", "type must be 'item_a'");
  UNIT_ASSERT_EQUAL((*first).type(), "item_a", "type must be 'item_a'");
  UNIT_ASSERT_EQUAL(first.get()->type(), "item_a", "type must be 'item_a'");
  first--;

  UNIT_ASSERT_TRUE(first == ptree.begin(), "expected prototype iterator to be begin()");
}

void PrototypeTreeTestUnit::test_relations()
{
  object_store ptree;

  object_store::const_iterator children_list_node = ptree.attach<children_list>("children_list");
  object_store::const_iterator master_node = ptree.attach<master>("master");
  object_store::const_iterator child_node = ptree.attach<child>("child");

  // get the relation table for children
  // holding id of children_list (container)
  // and id of child (value)
  object_store::const_iterator children_node = ptree.find("children");

  UNIT_ASSERT_EQUAL(child_node->relation_count(), 0UL, "relations must be empty");
  UNIT_ASSERT_EQUAL(child_node->foreign_key_count(), 0UL, "foreign keys must be empty");

  UNIT_ASSERT_EQUAL(children_list_node->relation_count(), 0UL, "relations must be empty");
  UNIT_ASSERT_EQUAL(children_list_node->foreign_key_count(), 0UL, "foreign keys must be empty");

  UNIT_ASSERT_EQUAL(master_node->relation_count(), 0UL, "relations must be empty");
  UNIT_ASSERT_GREATER(master_node->foreign_key_count(), 0UL, "foreign key must not be empty");
  UNIT_ASSERT_EQUAL(master_node->foreign_key_count(), 1UL, "there must be one foreign key");

//  prototype_node::t_foreign_key_map::const_iterator j = master_node->foreign_keys.find("child");
  UNIT_ASSERT_TRUE(master_node->has_foreign_key("child"), "iterator must not be end");
//  UNIT_ASSERT_FALSE(j == master_node->foreign_keys.end(), "iterator must not be end");

  UNIT_ASSERT_NOT_EQUAL(children_node->relation_count(), 0UL, "relations must not be empty");
  UNIT_ASSERT_EQUAL(children_node->relation_count(), 1UL, "there must be one relation");
//  prototype_node::field_prototype_map_t::const_iterator i = children_node->relations.find("children_list");
  UNIT_ASSERT_TRUE(children_node->has_relation("children_list"), "iterator must not be end");
//  UNIT_ASSERT_FALSE(i == children_node->relations.end(), "iterator must not be end");

  UNIT_ASSERT_NOT_EQUAL(children_node->foreign_key_count(), 0UL, "foreign keys must not be empty");
  // Todo: foreign key maybe take the id of the parent node in account
  UNIT_ASSERT_EQUAL(children_node->foreign_key_count(), 1UL, "there must be one foreign keys");
//  UNIT_ASSERT_EQUAL(children_node->foreign_key_count(), 2UL, "there must be two foreign keys");
//  j = children_node->foreign_keys.find("container");
//  UNIT_ASSERT_TRUE(children_node->has_foreign_key("owner_id"), "iterator must not be end");
//  j = children_node->foreign_keys.find("value");
  UNIT_ASSERT_TRUE(children_node->has_foreign_key("item_id"), "iterator must not be end");
}
