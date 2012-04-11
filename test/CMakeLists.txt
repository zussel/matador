SET (TEST_TOOLS_SOURCES
  tools/BlobTestUnit.hpp
  tools/BlobTestUnit.cpp
  tools/VarCharTestUnit.hpp
  tools/VarCharTestUnit.cpp
  tools/FactoryTestUnit.hpp
  tools/FactoryTestUnit.cpp
)

SET (TEST_OBJECT_SOURCES
  object/Item.hpp
  object/ObjectPrototypeTestUnit.cpp
  object/ObjectPrototypeTestUnit.hpp
  object/ObjectStoreTestUnit.cpp
  object/ObjectStoreTestUnit.hpp
  object/ObjectListTestUnit.cpp
  object/ObjectListTestUnit.hpp
  object/ObjectVectorTestUnit.cpp
  object/ObjectVectorTestUnit.hpp
  object/DatabaseTestUnit.cpp
  object/DatabaseTestUnit.hpp
)

SET (TEST_UNIT_SOURCES
  unit/FirstTestUnit.hpp
  unit/SecondTestUnit.hpp
)

SET (TEST_SOURCES test_oos.cpp)

#ADD_SUBDIRECTORY(object)
#ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(unit)

ADD_EXECUTABLE(test_oos ${TEST_SOURCES} ${TEST_TOOLS_SOURCES} ${TEST_OBJECT_SOURCES} ${TEST_UNIT_SOURCES})

TARGET_LINK_LIBRARIES(test_oos oos ${CMAKE_DL_LIBS})
