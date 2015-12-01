#include "object/prototype_tree.hpp"

#include "person.hpp"
#include "student.hpp"

using namespace oos;

int main()
{
  prototype_tree tree;

  tree.attach<person>("person");
  tree.attach<student, person>("student");

  prototype_iterator i = tree.find("person");

  i = tree.find<person>();

//  tree.insert(new person);
  
//  basic_proxy *bp = new proxy<person>;
//
//  i->insert(new person);

  return 0;
}