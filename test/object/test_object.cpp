#include "object/object_store.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/object_view.hpp"
#include "object/object_serializer.hpp"

#include "unit/unit_test.hpp"
#include "unit/test_suite.hpp"

#include "ObjectStoreTestUnit.hpp"
#include "ObjectPrototypeTestUnit.hpp"
#include "ComplexObjectStoreTestUnit.hpp"
#include "ObjectListTestUnit.hpp"
#include "DatabaseTestUnit.hpp"

#include <iostream>

using namespace std;
using namespace oos;

int
main(int argc, char *argv[])
{
  test_suite::instance().init(argc, argv);

  test_suite::instance().register_unit("prototype", new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit("objects", new ObjectStoreTestUnit());
  test_suite::instance().register_unit("complex", new ComplexObjectStoreTestUnit());
  test_suite::instance().register_unit("objectlist", new ObjectListTestUnit());
  test_suite::instance().register_unit("database", new DatabaseTestUnit());

	test_suite::instance().run();
}
