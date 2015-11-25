#include "object/prototype_tree.hpp"

#include "person.hpp"

using namespace oos;

int main()
{
  prototype_tree tree;

  tree.insert<person>("person");

  prototype_iterator i = tree.find("person");

  i = tree.find<person>();

//  tree.insert(new person);
  
//  basic_proxy *bp = new proxy<person>;
//
//  i->insert(new person);

  return 0;
}