SET (TEST_SOURCES
  test_object.cpp
  Item.hpp
  ObjectPrototypeTestUnit.cpp
  ObjectPrototypeTestUnit.hpp
  ObjectStoreTestUnit.cpp
  ObjectStoreTestUnit.hpp
  ObjectListTestUnit.cpp
  ObjectListTestUnit.hpp
  ObjectVectorTestUnit.cpp
  ObjectVectorTestUnit.hpp
  DatabaseTestUnit.cpp
  DatabaseTestUnit.hpp
)

ADD_EXECUTABLE(test_object ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(test_object oos ${CMAKE_DL_LIBS})
