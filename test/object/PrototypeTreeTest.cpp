#include "PrototypeTreeTest.hpp"
#include "../Item.hpp"

#include "object/prototype_tree.hpp"

using namespace oos;
using namespace std;

PrototypeTreeTestUnit::PrototypeTreeTestUnit()
  : unit_test("tree", "Prototype Tree Test Unit")
{
  add_test("empty", std::tr1::bind(&PrototypeTreeTestUnit::test_empty_tree, this), "test empty prototype tree");
  add_test("insert_root", std::tr1::bind(&PrototypeTreeTestUnit::test_insert_root_element, this), "test insert root element");
}

PrototypeTreeTestUnit::~PrototypeTreeTestUnit()
{}

void PrototypeTreeTestUnit::initialize()
{}

void PrototypeTreeTestUnit::finalize()
{}

void PrototypeTreeTestUnit::test_empty_tree()
{
  prototype_tree ptree;
  UNIT_ASSERT_TRUE(ptree.empty(), "prototype must be empty");
}

void PrototypeTreeTestUnit::test_insert_root_element()
{
  prototype_tree ptree;
  ptree.insert(new object_producer<Item>, "item", false);

  UNIT_ASSERT_TRUE(ptree.size() == 1, "prototype size must be one (1)");
}
