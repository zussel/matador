SET (TEST_SOURCES
  test_object.cpp
  Album.cpp
  Album.hpp
  Artist.cpp
  Artist.hpp
  Track.cpp
  Track.hpp
  ComplexObjectStoreTestUnit.cpp
  ComplexObjectStoreTestUnit.hpp
  ObjectPrototypeTestUnit.cpp
  ObjectPrototypeTestUnit.hpp
  ObjectStoreTestUnit.cpp
  ObjectStoreTestUnit.hpp
  ObjectListTestUnit.cpp
  ObjectListTestUnit.hpp
)

ADD_EXECUTABLE(test_object ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(test_object oos)
