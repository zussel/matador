#include "matador/utils/tree.hpp"
#include "TreeTest.hpp"


#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <utility>

using namespace std;
using namespace matador;

typedef tree<string> t_stringtree;
typedef list<string> t_stringlist;

template <typename T>
class equal_node_name : public unary_function<T, bool> {
public:
    explicit equal_node_name(T n = "") : name_(std::move(n)) {}
    bool operator() (const T& x) const {
        return x == name_;
    }
private:
    T name_;
};

template < typename T >
struct greater_name : public binary_function<typename tree<T>::t_node, typename tree<T>::t_node, bool> {
public:
  bool operator()(const typename tree<T>::t_node &a, const typename tree<T>::t_node &b) const {
    return (a.data > b.data);
  }
};

TreeTest::TreeTest()
  : unit_test("tree", "tree test")
{
  add_test("tree", [this] { test_tree(); }, "tree test");
}

void TreeTest::test_tree()
{
  t_stringtree stringtree;
  t_stringtree::iterator j, l, m, o, p;

  // add first node
  j = stringtree.insert(stringtree.end(), "hallo");

  UNIT_ASSERT_EQUAL("hallo", *j);
  l = j;
  UNIT_ASSERT_EQUAL("hallo", *l);

  // build up a tree
  stringtree.push_back_child(j, "welt");
  stringtree.push_back_child(j, "mars");
  o = stringtree.push_back_child(j, "mond zum verschieben");
  stringtree.push_back_child(j, "mond");
  stringtree.push_back_child(j, "mond");
  stringtree.push_back_child(j, "mond");
  m = stringtree.push_back_child(j, "mond muss weg");
  j = stringtree.push_back_child(j, "saturn");
  stringtree.push_back_child(j, "ring 1");
  p = stringtree.push_back_child(j, "ring 2");
  stringtree.push_back_child(j, "ring 3");

  UNIT_ASSERT_EQUAL(12, distance(stringtree.begin(), stringtree.end()));

  auto it = stringtree.begin();

  UNIT_ASSERT_EQUAL(0UL, stringtree.depth(it));
  UNIT_ASSERT_EQUAL("hallo", *it);

  ++it;

  UNIT_ASSERT_EQUAL(1UL, stringtree.depth(it));
  UNIT_ASSERT_EQUAL("welt", *it);

  int itcount = 0;
  std::for_each(stringtree.begin(), stringtree.end(), [&itcount](const std::string &) {++itcount;});

  UNIT_ASSERT_EQUAL(12, itcount);

  auto rit = stringtree.rbegin();

  UNIT_ASSERT_EQUAL(2UL, stringtree.depth(rit));
  UNIT_ASSERT_EQUAL("ring 3", *rit);

  ++rit;

  UNIT_ASSERT_EQUAL(2UL, stringtree.depth(rit));
  UNIT_ASSERT_EQUAL("ring 2", *rit);

  itcount = 0;
  std::for_each(stringtree.rbegin(), stringtree.rend(), [&itcount](const std::string &) {++itcount;});
  UNIT_ASSERT_EQUAL(12, itcount);

  auto sj = stringtree.begin(j);

  UNIT_ASSERT_EQUAL(2UL, stringtree.depth(sj));
  UNIT_ASSERT_EQUAL("ring 1", *sj);

  ++sj;

  UNIT_ASSERT_EQUAL(2UL, stringtree.depth(sj));
  UNIT_ASSERT_EQUAL("ring 2", *sj);

  // remove last "mond" node
  stringtree.erase(m);

  UNIT_ASSERT_EQUAL(11UL, stringtree.size());
  auto i = find_if(stringtree.begin(), stringtree.end(), equal_node_name<string>("mond muss weg"));
  UNIT_ASSERT_TRUE(i == stringtree.end());

  UNIT_ASSERT_EQUAL(1UL, stringtree.depth(o));
  // moving node "mond zum verschieben" before node "ring 2"
  stringtree.move(o, p);

  i = find_if(stringtree.begin(), stringtree.end(), equal_node_name<string>("mond zum verschieben"));
  UNIT_ASSERT_TRUE(i != stringtree.end());
  UNIT_ASSERT_EQUAL(2UL, stringtree.depth(i));

  auto si = find_if(stringtree.begin(j), stringtree.end(j), equal_node_name<string>("ring 2"));

  UNIT_ASSERT_TRUE(si != stringtree.end());

  UNIT_ASSERT_EQUAL(10UL, stringtree.size(l));

  t_stringlist sl;
  sl.push_back("hallo");
  sl.push_back("saturn");
  sl.push_back("ring 2");

  j = stringtree.find_in_path(sl.begin(), sl.end(), [](const std::string &elem, const std::string &path_elem) {
    return elem == path_elem;
  });

  UNIT_ASSERT_FALSE(j == stringtree.end());

  auto c = stringtree.begin();

  j = stringtree.begin();

  auto first = j;
  UNIT_ASSERT_EQUAL("welt", *(++first));
  stringtree.sort_children(j);
  first = j;
  UNIT_ASSERT_EQUAL("mars", *(++first));
  stringtree.sort_children(j, greater_name<string>());
  first = j;
  UNIT_ASSERT_EQUAL("welt", *(++first));

  t_stringtree::range_pair rp = stringtree.equal_range(j, equal_node_name<string>("mond"));

  UNIT_ASSERT_EQUAL(3, std::distance(rp.first, rp.second));

  j = std::find_if(stringtree.begin(), stringtree.end(), [](const std::string &val) {
    return val == "mond zum verschieben";
  });
  UNIT_ASSERT_FALSE(j == stringtree.end());
  UNIT_ASSERT_EQUAL("ring 2", *(++j));
  stringtree.sort();
  j = std::find_if(stringtree.begin(), stringtree.end(), [](const std::string &val) {
    return val == "mond zum verschieben";
  });
  UNIT_ASSERT_EQUAL("ring 1", *(++j));

  // clear tree
  stringtree.clear();

  UNIT_ASSERT_TRUE(stringtree.empty());

  // insert root
  j = stringtree.insert(stringtree.begin(), "root 1");

  // build up tree
  auto k = stringtree.push_back_child(j, "branch 1.1");
  m = stringtree.push_back_child(k, "branch 1.1.1");
  stringtree.push_back_child(m, "leaf 1.1.1.1");
  stringtree.push_back_child(m, "leaf 1.1.1.2");
  stringtree.push_back_child(k, "leaf 1.1.2");
  k = stringtree.push_back_child(j, "branch 1.2");
  stringtree.push_back_child(k, "leaf 1.2.1");
  m = stringtree.push_back_child(k, "branch 1.2.2");
  stringtree.push_back_child(m, "leaf 1.2.2.1");
  o = stringtree.push_back_child(m, "branch 1.2.2.2");
  stringtree.push_back_child(o, "leaf 1.2.2.2.1");
  stringtree.push_back_child(o, "leaf 1.2.2.2.2");
  stringtree.push_back_child(k, "leaf 1.2.3");
  k = stringtree.push_back_child(j, "branch 1.3");
  m = stringtree.push_back_child(k, "branch 1.3.1");
  stringtree.push_back_child(m, "leaf 1.3.1.1");
  m = stringtree.push_back_child(k, "branch 1.3.2");
  o = stringtree.push_back_child(m, "branch 1.3.2.1");
  stringtree.push_back_child(o, "leaf 1.3.2.1.1");
  p = stringtree.push_back_child(o, "branch 1.3.2.1.2");
  stringtree.push_back_child(p, "leaf 1.3.2.1.2.1");
  stringtree.push_back_child(m, "leaf 1.3.2.2");

  UNIT_ASSERT_EQUAL(23UL, stringtree.size());
  UNIT_ASSERT_EQUAL(12, std::distance(stringtree.begin_leaf(), stringtree.end_leaf()));

  itcount = 0;
  auto leaf_begin = stringtree.begin_leaf();
  for (auto bli = stringtree.end_leaf(); bli != leaf_begin; --bli) {
    ++itcount;
  }
  UNIT_ASSERT_EQUAL(12, itcount);
}
